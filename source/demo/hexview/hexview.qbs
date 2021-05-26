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
  name: "hexview"
  type: "staticlibrary"

  // dependencies
  Depends { name: "cpp" }
  Depends { name: "Qt.core" }
  Depends { name: "Qt.gui" }
  Depends { name: "Qt.widgets" }

  // cpp module configuration
  cpp.includePaths: [ "." ]
  cpp.cxxLanguageVersion: "c++14"
  cpp.useCxxPrecompiledHeader: true

  // export requirements
  Export {
    Depends { name: "cpp" }
    Depends { name: "Qt.core" }
    Depends { name: "Qt.gui" }
    Depends { name: "Qt.widgets" }
    cpp.includePaths: product.sourceDirectory
  }

  // install group for the produced executable
  Group {
    qbs.install: true
    fileTagsFilter: product.type
  }

  files: [
    "qhexview.h",
    "qhexview.cpp",
    "document/qhexcursor.cpp",
    "document/qhexcursor.h",
    "document/qhexdocument.cpp",
    "document/qhexdocument.h",
    "document/qhexmetadata.cpp",
    "document/qhexmetadata.h",
    "document/qhexrenderer.cpp",
    "document/qhexrenderer.h",
    "document/buffer/qfilebuffer.cpp",
    "document/buffer/qfilebuffer.h",
    "document/buffer/qhexbuffer.cpp",
    "document/buffer/qhexbuffer.h",
    "document/buffer/qmemorybuffer.cpp",
    "document/buffer/qmemorybuffer.h",
    "document/buffer/qmemoryrefbuffer.cpp",
    "document/buffer/qmemoryrefbuffer.h",
    "document/commands/hexcommand.cpp",
    "document/commands/hexcommand.h",
    "document/commands/insertcommand.cpp",
    "document/commands/insertcommand.h",
    "document/commands/removecommand.cpp",
    "document/commands/removecommand.h",
    "document/commands/replacecommand.cpp",
    "document/commands/replacecommand.h"
  ]
}
