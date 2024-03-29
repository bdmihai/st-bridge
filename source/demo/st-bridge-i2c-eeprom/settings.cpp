/*_____________________________________________________________________________
 |                                                                            |
 | COPYRIGHT (C) 2023 Mihai Baneu                                             |
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
 |  Author: Mihai Baneu                           Last modified: 11.Nov.2023  |
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

  setValue("Eeprom/Address", EEPROM_I2C_ADDRESS);
  setValue("Eeprom/Size", EEPROM_SIZE);
  setValue("Eeprom/PageSize", EEPROM_PAGE_SIZE);
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

quint8 Settings::getEepromAddress()
{
  return value("Eeprom/Address").toUInt();
}

void Settings::setEepromAddress(const quint8 &newValue)
{
    setValue("Eeprom/Address", newValue);
}

quint32 Settings::getEepromSize()
{
  return value("Eeprom/Size").toUInt();
}

void Settings::setEepromSize(const quint32 &newValue)
{
    setValue("Eeprom/Size", newValue);
}

quint32 Settings::getEepromPageSize()
{
  return value("Eeprom/PageSize").toUInt();
}

void Settings::setEepromPageSize(const quint32 &newValue)
{
    setValue("Eeprom/PageSize", newValue);
}
