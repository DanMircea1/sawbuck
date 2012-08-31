// Copyright 2012 Google Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "syzygy/grinder/lcov_writer.h"

#include "base/file_util.h"

namespace grinder {

bool LcovWriter::Add(const LineInfo& line_info) {
  // Multiple entries for the same source file are stored consecutively in
  // the LineInfo, hence we use this as a cache to prevent repeated lookups
  // of source file names in our SourceFileCoverageInfoMap.
  const std::string* old_source_file_name = NULL;
  SourceFileCoverageInfoMap::iterator source_it;

  LineInfo::SourceLines::const_iterator line_it =
      line_info.source_lines().begin();
  for (; line_it != line_info.source_lines().end(); ++line_it) {
    DCHECK(line_it->source_file_name != NULL);

    // Different source file? Then insert/lookup in our map.
    if (old_source_file_name != line_it->source_file_name) {
      // We don't care whether it already exists or not.
      source_it = source_file_coverage_info_map_.insert(
          std::make_pair(*line_it->source_file_name,
                         CoverageInfo())).first;
      old_source_file_name = line_it->source_file_name;
    }

    // Insert/lookup the execution count by line number.
    LineExecutionCountMap::iterator line_exec_it =
        source_it->second.line_execution_count_map.insert(
            std::make_pair(line_it->line_number, 0)).first;

    // Set the execution count. Since LineInfo only has a 'visited' boolean,
    // we simply leave the execution count as initialized, or set it to 1.
    if (line_it->visited)
      line_exec_it->second = 1;
  }

  return true;
}

bool LcovWriter::Write(const FilePath& path) const {
  file_util::ScopedFILE file(file_util::OpenFile(path, "wb"));
  if (file.get() == NULL) {
    LOG(ERROR) << "Failed to open file for writing: " << path.value();
    return false;
  }

  SourceFileCoverageInfoMap::const_iterator source_it =
      source_file_coverage_info_map_.begin();
  for (; source_it != source_file_coverage_info_map_.end(); ++source_it) {
    ::fprintf(file.get(), "SF:%s\n", source_it->first.c_str());

    // Iterate over the line execution data, keeping summary statistics as we
    // go.
    size_t lines_executed = 0;
    LineExecutionCountMap::const_iterator line_it =
        source_it->second.line_execution_count_map.begin();
    LineExecutionCountMap::const_iterator line_it_end =
        source_it->second.line_execution_count_map.end();
    for (; line_it != line_it_end; ++line_it) {
      ::fprintf(file.get(), "DA:%d,%d\n", line_it->first, line_it->second);
      if (line_it->second > 0)
        ++lines_executed;
    }

    // Output the summary statistics for this file.
    ::fprintf(file.get(), "LH:%d\n", lines_executed);
    ::fprintf(file.get(), "LF:%d\n",
              source_it->second.line_execution_count_map.size());

    ::fprintf(file.get(), "end_of_record\n");
  }

  return true;
}

}  // namespace grinder