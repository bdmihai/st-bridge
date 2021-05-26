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
 |  Author: Mihai Baneu                           Last modified: 10.Dec.2020  |
 |                                                                            |
 |___________________________________________________________________________*/

import qbs 1.0

Product {
  name: "st-bridge-gpio-doc"
  type: "qhc"

  Depends { name: "Qt.core" }

  Qt.core.qdocEnvironment: [
  ]

  files: [
    "config/**/*.qdocconf",
    "documentation/**/*.qdoc",
    "templates/**/*.*",
    "about.txt"
  ]

  Group {
    name: "qdocconf-main"
    files: "st-bridge-gpio-doc.qdocconf"
    fileTags: "qdocconf-main"
  }

  Group {
    name: "qhcp"
    files: [
          "st-bridge-gpio-doc.qhcp",
      ]
    fileTags: "qhcp"
  }

  Group {
    fileTagsFilter: ["qch", "qhc"]
    qbs.install: true
  }

  Rule {
    inputs: "qhcp"
    auxiliaryInputs: ["qch"]
    Artifact {
      filePath: input.completeBaseName + ".qhc"
      fileTags: ["qhc"]
    }

    prepare: {
      var args = [input.filePath, '-o', output.filePath];
      var cmd = new Command('/usr/bin/qhelpgenerator', args);

      cmd.description = 'qhelpgenerator  ' + input.fileName;
      cmd.highlight = 'filegen';
      return cmd;
    }
  }
}
