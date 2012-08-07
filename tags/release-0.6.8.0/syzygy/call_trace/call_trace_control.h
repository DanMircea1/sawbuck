// Copyright 2011 Google Inc.
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
#ifndef SYZYGY_CALL_TRACE_CALL_TRACE_CONTROL_H_
#define SYZYGY_CALL_TRACE_CALL_TRACE_CONTROL_H_

#include <windows.h>

extern "C" {

void CALLBACK BeginCallTrace(HWND unused_window,
                             HINSTANCE unused_instance,
                             LPSTR unused_cmd_line,
                             int unused_show);

}  // extern "C"

#endif  // SYZYGY_CALL_TRACE_CALL_TRACE_CONTROL_H_