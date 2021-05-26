/*_____________________________________________________________________________
 |                                                                            |
 | COPYRIGHT (C) 2020 Mihai Baneu                                             |
 |                                                                            |
 | Permission is hereby  granted,  free of charge,  to any person obtaining a |
 | copy of this software and associated documentation files (the "Software"), |
 | to deal in the Software without restriction,  including without limitation |
 | the rights to  use, copy, modify, merge, publish, distribute,  sublicense, |
 | and/or sell copies  of  the Software, and to permit  persons to  whom  the |
 | Software is furnished to do so, subject to the following conditions:       |
 |                                                                            |
 | The above  copyright notice  and this permission notice  shall be included |
 | in all copies or substantial portions of the Software.                     |
 |                                                                            |
 | THE SOFTWARE IS PROVIDED  "AS IS",  WITHOUT WARRANTY OF ANY KIND,  EXPRESS |
 | OR   IMPLIED,   INCLUDING   BUT   NOT   LIMITED   TO   THE  WARRANTIES  OF |
 | MERCHANTABILITY,  FITNESS FOR  A  PARTICULAR  PURPOSE AND NONINFRINGEMENT. |
 | IN NO  EVENT SHALL  THE AUTHORS  OR  COPYRIGHT  HOLDERS  BE LIABLE FOR ANY |
 | CLAIM, DAMAGES OR OTHER LIABILITY,  WHETHER IN AN ACTION OF CONTRACT, TORT |
 | OR OTHERWISE, ARISING FROM,  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  |
 | THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                 |
 |____________________________________________________________________________|
 |                                                                            |
 |  Author: Mihai Baneu                           Last modified: 18.Oct.2020  |
 |                                                                            |
 |___________________________________________________________________________*/

import qbs

Product {
  name: "st-bridge"
  type: "dynamiclibrary"

  // dependencies
  Depends { name: "cpp" }

  // cpp module configuration
  cpp.includePaths: [
    ".",
    "./bridge",
    "./common",
    "./error"
  ]
  cpp.cxxLanguageVersion: "c++14"
  cpp.useCxxPrecompiledHeader: true
  cpp.dynamicLibraries: [ "usb-1.0" ]

  // export requirements
  Export {
    Depends { name: "cpp" }
    cpp.includePaths: product.cpp.includePaths
    cpp.dynamicLibraries: product.cpp.dynamicLibraries
  }

  // install group for the produced executable
  Group {
    qbs.install: true
    fileTagsFilter: product.type
  }

  files: [
    "bridge/*.h",
    "bridge/*.cpp",
    "common/*.h",
    "common/*.cpp",
    "error/*.h",
    "error/*.cpp"
  ]
}
