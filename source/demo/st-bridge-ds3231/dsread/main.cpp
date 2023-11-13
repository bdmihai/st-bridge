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
#include "defines.h"

const char days[7][4] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};

inline uint8_t bcd_to_dec(uint8_t bcd)
{
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

inline uint8_t dec_to_bcd(uint8_t dec)
{
    return ((((dec / 10) & 0xF) << 4) |
            ((dec % 10) & 0xF));
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    STLinkInterface *stInterface;
    Brg *brg;
    STLink_DeviceInfo2T selectedDevice;

    // set the application info
    app.setApplicationName(APP_NAME);
    app.setOrganizationName(APP_COMPANY);
    app.setOrganizationDomain(APP_DOMAIN);
    app.setApplicationVersion(APP_VERSION);

    // application options
    QCommandLineParser parser;
    parser.setApplicationDescription(APP_NAME " " APP_VERSION);
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
        {{"d", "device"}, "ST-Link device SN", "device_sn", ""    },
        {{"i", "init"  }, "Initialize with current date and time" },
        {"verbose"      , "Print debug information"               }
    });
    parser.process(app);

    if (!parser.isSet("verbose")) {
        qSetMessagePattern("%{if-debug}%{message}%{endif}%{if-info}%{message}%{endif}%{if-warning}%{message}%{endif}%{if-critical}%{message}%{endif}%{if-fatal}%{message}%{endif}");
    }

    qInfo().noquote() << APP_NAME << "version" << APP_VERSION;
    qInfo().noquote() << "Copyright (C) " << APP_COMPANY << "All rights reserved." << Qt::endl;

    uint32_t devCnt = 0;
    stInterface = new STLinkInterface();
    stInterface->LoadStlinkLibrary("");
    stInterface->EnumDevices(&devCnt, true);
    qInfo().noquote() << "Found" << devCnt << QString("ST-Link Devices (taking %1):").arg(parser.value("device"));
    for (int i = 0; i < (int)devCnt; i++) {
        STLink_DeviceInfo2T devInfo;
        if (stInterface->GetDeviceInfo2(i, &devInfo, 0) == STLINKIF_NO_ERR) {
            qInfo().noquote() << "ST-Link device #" << i << ":" << devInfo.EnumUniqueId;
            if (parser.value("device").isEmpty() || (parser.value("device") == devInfo.EnumUniqueId)) {
                selectedDevice = devInfo;
            }
        }
    }

    brg = new Brg(*stInterface);
    Brg_StatusT brg_status = brg->OpenStlink(selectedDevice.EnumUniqueId, true);

    if (brg_status != BRG_NO_ERR) {
        qCritical().noquote() << "Device open failed with error:" << brg_status;
        return -1;
    }
    qInfo().noquote() << "Device opened successfully";

    // double check if the bridge is connected
    if (!brg->GetIsStlinkConnected()) {
        qCritical().noquote() << "Device is not connected!";
        return -1;
    }

    Brg_StatusT status;
    Brg_I2cInitT param;
    status = brg->GetI2cTiming(I2C_STANDARD, // I2CSpeedMode  #I2C_STANDARD, #I2C_FAST, #I2C_FAST_PLUS
                               100,      // In KHz, 1-100KHz (STANDARD), 1-400KHz (FAST), 1-1000KHz (FAST PLUS)
                               0,        // DNFn  0 (no digital filter) up to 15, noise digital filter (delay = DNFn/SpeedFrequency)
                               300,      // RiseTime In ns, 0-1000ns (STANDARD), 0-300ns (FAST), 0-120ns (FAST PLUS)
                               300,      // FallTime In ns, 0-300ns (STANDARD), 0-300ns (FAST), 0-120ns (FAST PLUS)
                               true,     // Use true for Analog Filter ON or false for Analog Filter OFF
                               &param.TimingReg);
    if (status != BRG_NO_ERR) {
        qCritical().noquote() << "Timing parameters error:" << status;
        return -1;
    }
    param.OwnAddr = 0;
    param.AddrMode = I2C_ADDR_7BIT;
    param.AnFilterEn = I2C_FILTER_ENABLE;
    param.DigitalFilterEn = I2C_FILTER_DISABLE;
    param.Dnf = 0;

    status = brg->InitI2C(&param);
    if (status != BRG_NO_ERR) {
        qCritical().noquote() << "I2C Initialization failed:" << status;
        return -1;
    }

    if (parser.isSet("init")) {
        QDateTime now = QDateTime::currentDateTime();
        uint8_t init[] = { 0,
            dec_to_bcd(now.time().second()), dec_to_bcd(now.time().minute()), dec_to_bcd(now.time().hour()),
            (uint8_t)now.date().dayOfWeek(), dec_to_bcd(now.date().day()), (uint8_t)(0x80 | dec_to_bcd(now.date().month())), dec_to_bcd(now.date().year() % 100)
        };

        status = brg->WriteI2C(init, 0b01101000, sizeof(init), 0);
        if (status != BRG_NO_ERR) {
            qCritical().noquote() << "WriteI2C initial time failed:" << status;
            return -1;
        }
        QThread::msleep(1000);
        qInfo().noquote() << "Time set to:" << now.toString();
    }

    uint8_t pointer[] = { 0x00 };
    status = brg->WriteI2C(pointer, 0b01101000, sizeof(pointer), 0);
    if (status != BRG_NO_ERR) {
        qCritical().noquote() << "WriteI2C pointer failed:" << status;
        return -1;
    }

    uint8_t buffer[0x13] = { 0x00 };
    status = brg->ReadI2C(buffer, 0b01101000, sizeof(buffer), 0);
    if (status != BRG_NO_ERR) {
        qCritical().noquote() << "ReadI2C time buffer failed:" << status;
        return -1;
    }

    qInfo().noquote() << QString("Date: %1 %2-%3-%4%5")
                      .arg(days[buffer[3] - 1])
                      .arg(bcd_to_dec(buffer[4]), 2, 10, QChar('0'))
                      .arg(bcd_to_dec(buffer[5] & 0x7F), 2, 10, QChar('0'))
                      .arg(buffer[5] & 0x80 ? 20 : 19)
                      .arg(bcd_to_dec(buffer[6]), 2, 10, QChar('0'));
    qInfo().noquote() << QString("Time: %1:%2:%3")
                      .arg(bcd_to_dec(buffer[2]), 2, 10, QChar('0'))
                      .arg(bcd_to_dec(buffer[1]), 2, 10, QChar('0'))
                      .arg(bcd_to_dec(buffer[0]), 2, 10, QChar('0'));

    qDebug().noquote() << "Registers:";
    for (size_t i = 0; i < sizeof(buffer); i++) {
        qDebug().noquote() << QString("0x%1\t0x%2").arg(i, 2, 16, QChar('0')).arg(buffer[i], 8, 2, QChar('0'));
    }

    brg->CloseStlink();
    delete brg;
    delete stInterface;

    return 0;
}
