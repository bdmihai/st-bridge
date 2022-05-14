/*_____________________________________________________________________________
 |                                                                            |
 | COPYRIGHT (C) 2021 Mihai Baneu                                             |
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
 |  Author: Mihai Baneu                           Last modified: 26.Mai.2021  |
 |                                                                            |
 |___________________________________________________________________________*/

#include "stable.h"
#include "settings.h"
#include "defines.h"
#include "main_window.h"
#include "qhexview.h"
#include "device_combo_box.h"
#include "document/buffer/qmemorybuffer.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_helpProcess(nullptr)
{
  isFirstTime = true;
  m_stInterface = new STLinkInterface();
  m_stInterface->LoadStlinkLibrary("");
  m_brg = new Brg(*m_stInterface);

  createLayout();
  createActions();
  createMenus();
  createToolBars();
  createDockWindows();
  createStatusBar();
}

MainWindow::~MainWindow()
{
  m_brg->CloseStlink();
  delete m_brg;
  delete m_stInterface;
}

void MainWindow::showEvent(QShowEvent *event)
{
  if (isFirstTime) {
    readSettings();
    isFirstTime = false;
    refresh();
  }
  QMainWindow::showEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (1) {
    writeSettings();

    // close any possible help
    if (m_helpProcess)
      m_helpProcess->terminate();

    event->accept();
  } else {
    event->ignore();
  }
}

void MainWindow::createLayout()
{
  m_deviceComboBox = new DeviceComboBox(this);

  m_mainView = new QHexView(this);

  QFont font("Source Code Pro", 11);
  m_mainView->setFont(font);

  QByteArray bytearray(EEPROM_SIZE, 0x00);
  m_document = QHexDocument::fromMemory<QMemoryBuffer>(bytearray);
  m_document->setBaseAddress(EEPROM_I2C_ADDRESS * 256);
  m_mainView->setDocument(m_document);

  setCentralWidget(m_mainView);
  setWindowIcon(QIcon(":/images/st-bridge-gpio.svg"));
}

void MainWindow::createActions()
{
  startAct = new QAction(QIcon(":/images/start.svg"), "&Start", this);
  startAct->setStatusTip("Start controlling the GPIO's");
  startAct->setDisabled(true);
  connect(startAct, SIGNAL(triggered()), this, SLOT(start()));

  stopAct = new QAction(QIcon(":/images/stop.svg"), "S&top", this);
  stopAct->setStatusTip("Stop the GPIO controlling");
  stopAct->setDisabled(true);
  connect(stopAct, SIGNAL(triggered()), this, SLOT(stop()));

  refreshAct = new QAction("Scan", this);
  refreshAct->setStatusTip("Refresh the list of devices");
  connect(refreshAct, SIGNAL(triggered()), this, SLOT(refresh()));

  readAct = new QAction(QIcon(":/images/eeprom-read.png"), "Read eeprom", this);
  readAct->setStatusTip("Read the eeprom content");
  readAct->setDisabled(false);
  connect(readAct, SIGNAL(triggered()), this, SLOT(readData()));

  writeAct = new QAction(QIcon(":/images/eeprom-write.png"), "Write eeprom", this);
  writeAct->setStatusTip("Write the eeprom content");
  writeAct->setDisabled(false);
  connect(writeAct, SIGNAL(triggered()), this, SLOT(writeData()));

  exitAct = new QAction("E&xit", this);
  exitAct->setShortcuts(QKeySequence::Quit);
  exitAct->setStatusTip("Exit the application");
  connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

  helpAct = new QAction("&Help...", this);
  helpAct->setStatusTip("Help");
  connect(helpAct, SIGNAL(triggered()), this, SLOT(help()));

  aboutAct = new QAction("&About...", this);
  aboutAct->setStatusTip("About");
  connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
}

void MainWindow::createMenus()
{
  m_fileMenu = menuBar()->addMenu("&File");
  m_fileMenu->addAction(startAct);
  m_fileMenu->addAction(stopAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(refreshAct);
  m_fileMenu->addSeparator();
  m_fileMenu->addAction(exitAct);

  m_helpMenu = menuBar()->addMenu("&Help");
  m_helpMenu->addAction(helpAct);
  m_helpMenu->addSeparator();
  m_helpMenu->addAction(aboutAct);
}

void MainWindow::createToolBars()
{
  m_mainToolBar = addToolBar("Toolbar");
  m_mainToolBar->setObjectName("QToolBar.Toolbar");
  m_mainToolBar->setIconSize(QSize(24, 24));
  m_mainToolBar->setMovable(false);
  m_mainToolBar->addAction(startAct);
  m_mainToolBar->addAction(stopAct);
  m_mainToolBar->addSeparator();
  m_mainToolBar->addWidget(m_deviceComboBox);
  m_mainToolBar->addAction(refreshAct);
  m_mainToolBar->addSeparator();
  m_mainToolBar->addAction(readAct);
  m_mainToolBar->addAction(writeAct);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage("Ready");
}

void MainWindow::createDockWindows()
{
  setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
  setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}

void MainWindow::readSettings()
{
  restoreGeometry(settings.getWindowGeometry());
  restoreState(settings.getWindowState());
}

void MainWindow::writeSettings()
{
  settings.setWindowGeometry(saveGeometry());
  settings.setWindowState(saveState());
}

void MainWindow::updateWidgets()
{
  if (m_deviceComboBox->count() > 0) {
    if (m_brg->GetIsStlinkConnected()) {
      startAct->setEnabled(false);
      stopAct->setEnabled(true);
      readAct->setEnabled(true);
      writeAct->setEnabled(true);
    } else {
      startAct->setEnabled(true);
      stopAct->setEnabled(false);
      readAct->setEnabled(false);
      writeAct->setEnabled(false);
    }
  } else {
    startAct->setEnabled(false);
    stopAct->setEnabled(false);
    readAct->setEnabled(false);
    writeAct->setEnabled(false);
  }
}

void MainWindow::start()
{
  Brg_StatusT ret = m_brg->OpenStlink(m_deviceComboBox->currentText().toUtf8().constData(), true);

  updateWidgets();

  if (ret == BRG_NO_ERR) {
    statusBar()->showMessage(tr("Device opened successfully"));
  } else {
    statusBar()->showMessage(tr("Device open failed with error: %1").arg(ret));
  }
}

void MainWindow::stop()
{
  m_brg->CloseStlink();
  updateWidgets();
  statusBar()->showMessage(tr("Device closed."));
}

void MainWindow::refresh()
{
  uint32_t devCnt = 0;

  // stop the bridge connection if a bridge is already made
  if (m_brg->GetIsStlinkConnected()) {
    stop();
  }

  // fill the combo box with found devices
  m_deviceComboBox->clear();
  m_stInterface->EnumDevices(&devCnt, true);
  for (int i = 0; i < (int)devCnt; i++) {
    STLink_DeviceInfo2T devInfo;
    if (m_stInterface->GetDeviceInfo2(i, &devInfo, 0) == STLINKIF_NO_ERR) {
      m_deviceComboBox->addItem(devInfo.EnumUniqueId);
    }
  }

  updateWidgets();
}

void MainWindow::readData()
{
  if (m_brg->GetIsStlinkConnected()) {
    Brg_StatusT status;
    Brg_I2cInitT param;

    status = m_brg->GetI2cTiming(I2C_FAST, 400, 0, 300, 300, true, &param.TimingReg);
    if (status == BRG_NO_ERR) {
      param.OwnAddr = 0;
      param.AddrMode = I2C_ADDR_7BIT;
      param.AnFilterEn = I2C_FILTER_ENABLE;
      param.DigitalFilterEn = I2C_FILTER_DISABLE;
      param.Dnf = 0;

      status = m_brg->InitI2C(&param);
      if (status == BRG_NO_ERR) {
        QByteArray buffer(EEPROM_SIZE, 0x00);
        uint8_t address[] = { /*LSB*/ 0x00, /*MSB*/ 0x00 };

        status = m_brg->WriteI2C(address, EEPROM_I2C_ADDRESS + address[1], 1, 0);
        if (status == BRG_NO_ERR) {
          status = m_brg->ReadI2C((uint8_t *)(buffer.data()), 0b1010000, buffer.size(), 0);
          if (status == BRG_NO_ERR) {
            m_document = QHexDocument::fromMemory<QMemoryBuffer>(buffer);
            m_document->setBaseAddress(EEPROM_I2C_ADDRESS * 256);
            m_mainView->setDocument(m_document);
            statusBar()->showMessage(tr("Read %2 bytes from 0x%1").arg(EEPROM_I2C_ADDRESS * 256, 4, 16, QChar('0')).arg(EEPROM_SIZE));
          } else {
            statusBar()->showMessage(tr("Read over I2C failed: %1").arg(status));
          }
        } else {
          statusBar()->showMessage(tr("Address set over I2C failed: %1").arg(status));
        }

      } else {
        statusBar()->showMessage(tr("Initialization of the I2C failed: %1").arg(status));
      }
    } else {
      statusBar()->showMessage(tr("I2C timing error, timing reg: %1").arg(status));
    }
  }
}

void MainWindow::writeData()
{
  if (m_brg->GetIsStlinkConnected()) {
    Brg_StatusT status;
    Brg_I2cInitT param;

    status = m_brg->GetI2cTiming(I2C_FAST, 400, 0, 300, 300, true, &param.TimingReg);
    if (status == BRG_NO_ERR) {
      param.OwnAddr = 0;
      param.AddrMode = I2C_ADDR_7BIT;
      param.AnFilterEn = I2C_FILTER_ENABLE;
      param.DigitalFilterEn = I2C_FILTER_DISABLE;
      param.Dnf = 0;

      status = m_brg->InitI2C(&param);
      if (status == BRG_NO_ERR) {
        QBuffer ioBuffer;

        // save the document
        ioBuffer.open(QIODevice::WriteOnly);
        m_document->saveTo(&ioBuffer);
        ioBuffer.close();
        QByteArray buffer(ioBuffer.data());

        // only the eeprom size will be counted - less bytes are possible
        uint16_t bufferSize = std::min((int)buffer.size(), EEPROM_SIZE);
        for (uint16_t i = 0; i < bufferSize; i += EEPROM_PAGE_SIZE) {
          uint8_t pageBuffer[EEPROM_PAGE_SIZE + 1];
          uint16_t pageSize;

          pageSize = ((bufferSize - i) >= EEPROM_PAGE_SIZE) ? EEPROM_PAGE_SIZE : (bufferSize - i);
          memcpy(pageBuffer + 1, buffer.constData() + i, pageSize);
          pageBuffer[0] = (uint8_t)i;

          status = m_brg->WriteI2C(pageBuffer, EEPROM_I2C_ADDRESS + (uint8_t)(i >> 8), pageSize + 1, 0);
          if (status == BRG_NO_ERR) {
            QThread::msleep(10);
          } else {
            statusBar()->showMessage(tr("Address set over I2C failed: %1").arg(status));
            break;
          }

          if (status == BRG_NO_ERR) {
            statusBar()->showMessage(tr("Written %2 bytes at 0x%1").arg(EEPROM_I2C_ADDRESS * 256, 4, 16, QChar('0')).arg(bufferSize));
            QThread::msleep(10);
          }
        }
      } else {
        statusBar()->showMessage(tr("Initialization of the I2C failed: %1").arg(status));
      }
    } else {
      statusBar()->showMessage(tr("I2C timing error, timing reg: %1").arg(status));
    }
  }
}

void MainWindow::about()
{
  QMessageBox::about(this,
                     QString("About ") + APP_NAME,
                     QString("<b>") + APP_NAME + "</b> version " + APP_VERSION + "<p>" +
                     QString(APP_COMPANY) + "</p>" +
                     "<p>Email for questions and support: <a href=\"mailto:bdmihai@gmail.com.com\">bdmihai@gmail.com.com</a></p>" +
                     "<p>Open source software:</p>"
                     "<ul>"
                     "<li><a href=\"https://www.qt.io\">Qt cross-platform development framework</a> - Version " + qVersion() + "</li>"
                     "<p>License: GNU Lesser General Public License - https://www.gnu.org/copyleft/lesser.html</p>"
                     "</ul>"
                     "<p>Other software:</p>"
                     "<ul>"
                     "<li><a href=\"https://www.st.com/en/development-tools/stlink-v3-bridge.html\">stlink-v3-bridge</a></li>"
                     "<li><a href=\"https://github.com/martonmiklos/STLINK-V3-BRIDGE_libusb\">libust st-link bridge implementation</a></li>"
                     "<p>License: User-friendly ULTIMATE LIBERTY license agreement - https://www.st.com/SLA0044</p>"
                     "<li><a href=\"https://github.com/Dax89/QHexView\">QHexView - hexadecimal widget for Qt5</a></li>"
                     "<p>License: MIT License</p>"
                     "</ul>");
}

void MainWindow::help()
{
  QStringList args;
  static bool firstShow = true;

  if (m_helpProcess == 0) {
    m_helpProcess = new QProcess(this);

    if (firstShow) {
      args << "-collectionFile"
           << qApp->applicationDirPath() + "/st-bridge-gpio-doc.qhc"
           << "-register"
           << "st-bridge-gpio-doc.qch"
           << "-quiet";

      connect(m_helpProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this](int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/) {
        m_helpProcess->deleteLater();
        m_helpProcess = nullptr;
        this->help();
      });
    } else {
      args << "-collectionFile"
           << qApp->applicationDirPath() + "/st-bridge-gpio-doc.qhc"
           << "-enableRemoteControl";

      connect(m_helpProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), [this](int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/) {
        m_helpProcess->deleteLater();
        m_helpProcess = nullptr;
      });
    }

    m_helpProcess->start("/usr/bin/assistant", args);
    if (!m_helpProcess->waitForStarted(5000)) {
      m_helpProcess = nullptr;
    }
    firstShow = false;
  }
}
