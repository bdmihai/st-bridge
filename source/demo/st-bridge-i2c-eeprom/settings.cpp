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

#include "stable.h"
#include "settings.h"
#include "defines.h"

Settings settings(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + "/" + APP_NAME + "/" + "Settings.ini");

Settings::Settings(QString filePath) : QSettings(filePath, QSettings::IniFormat, 0)
{
  if (!QFile::exists(filePath))
    setDefault();
}

Settings::~Settings()
{
}

void Settings::setDefault()
{
  setValue("APP_NAME", APP_NAME);
  setValue("APP_VERSION", APP_VERSION);
  setValue("APP_COMPANY", APP_COMPANY);
  setValue("APP_DOMAIN", APP_DOMAIN);
}

void Settings::setWindowGeometry(const QByteArray &newValue)
{
  setValue("MainWindow/Geometry", newValue);
}

QByteArray Settings::getWindowGeometry()
{
  return value("MainWindow/Geometry").toByteArray();
}

void Settings::setWindowState(const QByteArray &newValue)
{
  setValue("MainWindow/State", newValue);
}

QByteArray Settings::getWindowState()
{
  return value("MainWindow/State").toByteArray();
}
