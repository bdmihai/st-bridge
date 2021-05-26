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
  name: "st-bridge-i2c-eeprom"
  type: "application"
  consoleApplication: false

  // dependencies
  Depends { name: "cpp" }
  Depends { name: "Qt.core" }
  Depends { name: "Qt.gui" }
  Depends { name: "Qt.widgets" }
  Depends { name: "st-bridge" }
  Depends { name: "hexview" }

  // cpp module configuration
  cpp.includePaths: [ ".",  ]
  cpp.cxxLanguageVersion: "c++14"
  cpp.useCxxPrecompiledHeader: true

  // qt core properties
  Qt.core.resourceSourceBase: sourceDirectory

  // precompiled header
  Group {
    name: "pch files"
    files: [ "stable.h" ]
    fileTags: "cpp_pch_src"
  }

  // images resource files
  Group {
    name: "images"
    files : [ "images/*.png", "images/*.svg", "images/*.ico" ]
    fileTags: "qt.core.resource_data"
  }

  // install group for the produced executable
  Group {
    qbs.install: true
    fileTagsFilter: product.type
  }

  files: [
        "defines.h",
        "device_combo_box.cpp",
        "device_combo_box.h",
        "main.cpp",
        "settings.cpp",
        "settings.h",
        "main_window.cpp",
        "main_window.h",
    ]
}
