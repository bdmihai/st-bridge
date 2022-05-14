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
#include "main_window.h"
#include "gpio_widget.h"
#include "device_combo_box.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_helpProcess(nullptr)
{
  isFirstTime = true;
  m_widgetsActive = false;
  m_stInterface = new STLinkInterface();
  m_stInterface->LoadStlinkLibrary("");
  m_brg = new Brg(*m_stInterface);

  m_pollTimer.setSingleShot(false);
  m_pollTimer.setInterval(10);
  connect(&m_pollTimer, &QTimer::timeout, this, &MainWindow::readData);

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
  QWidget *centralWidget = new QWidget();
  QHBoxLayout *layout = new QHBoxLayout();

  m_deviceComboBox = new DeviceComboBox(this);

  m_gpioWidget[0] = new GpioWidget("GPIO 0", BRG_GPIO_0);
  m_gpioWidget[1] = new GpioWidget("GPIO 1", BRG_GPIO_1);
  m_gpioWidget[2] = new GpioWidget("GPIO 2", BRG_GPIO_2);
  m_gpioWidget[3] = new GpioWidget("GPIO 3", BRG_GPIO_3);

  connect(m_gpioWidget[0], &GpioWidget::configChanged, this, &MainWindow::changeConfig);
  connect(m_gpioWidget[1], &GpioWidget::configChanged, this, &MainWindow::changeConfig);
  connect(m_gpioWidget[2], &GpioWidget::configChanged, this, &MainWindow::changeConfig);
  connect(m_gpioWidget[3], &GpioWidget::configChanged, this, &MainWindow::changeConfig);

  connect(m_gpioWidget[0], &GpioWidget::valueChanged, this, &MainWindow::changeValue);
  connect(m_gpioWidget[1], &GpioWidget::valueChanged, this, &MainWindow::changeValue);
  connect(m_gpioWidget[2], &GpioWidget::valueChanged, this, &MainWindow::changeValue);
  connect(m_gpioWidget[3], &GpioWidget::valueChanged, this, &MainWindow::changeValue);

  layout->addWidget(m_gpioWidget[0]);
  layout->addWidget(m_gpioWidget[1]);
  layout->addWidget(m_gpioWidget[2]);
  layout->addWidget(m_gpioWidget[3]);

  centralWidget->setLayout(layout);

  setCentralWidget(centralWidget);
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

void MainWindow::start()
{
  Brg_StatusT ret = m_brg->OpenStlink(m_deviceComboBox->currentText().toUtf8().constData(), true);

  if (ret == BRG_NO_ERR) {
    readData();
    statusBar()->showMessage(tr("Device opened successfully"));
  } else {
    readData();
    statusBar()->showMessage(tr("Device open failed with error: %1").arg(ret));
  }
}

void MainWindow::stop()
{
  m_brg->CloseStlink();
  readData();
  statusBar()->showMessage(tr("Device closed."));
}

void MainWindow::refresh()
{
  uint32_t devCnt = 0;

  m_deviceComboBox->clear();
  m_stInterface->EnumDevices(&devCnt, true);
  for (int i = 0; i < (int)devCnt; i++) {
    STLink_DeviceInfo2T devInfo;
    if (m_stInterface->GetDeviceInfo2(i, &devInfo, 0) == STLINKIF_NO_ERR) {
      m_deviceComboBox->addItem(devInfo.EnumUniqueId);
    }
  }

  if ((m_deviceComboBox->count()) > 0 && (!m_widgetsActive)) {
    startAct->setEnabled(true);
    stopAct->setEnabled(false);
  }
}

void MainWindow::readData()
{
  if (m_brg->GetIsStlinkConnected()) {
    if (!m_widgetsActive) {
      for (int i: {0, 1, 2, 3}) {
        m_gpioWidget[i]->setGpioMode(GPIO_MODE_INPUT);
        m_gpioWidget[i]->setGpioPull(GPIO_NO_PULL);
        m_gpioWidget[i]->setGpioOutput(GPIO_OUTPUT_PUSHPULL);
        m_gpioWidget[i]->setGpioSpeed(GPIO_SPEED_LOW);
        m_gpioWidget[i]->setGpioVal(GPIO_RESET);
        m_gpioWidget[i]->setActive(true);
      }
      m_widgetsActive = true;
      changeConfig();

      m_pollTimer.start();
      startAct->setEnabled(false);
      stopAct->setEnabled(true);
    }
    if (!readValues()) {
      m_brg->CloseStlink();
      refresh();
    }
  } else {
    if (m_widgetsActive) {
      for (int i: {0, 1, 2, 3}) {
        m_gpioWidget[i]->setGpioMode(GPIO_MODE_INPUT);
        m_gpioWidget[i]->setGpioPull(GPIO_NO_PULL);
        m_gpioWidget[i]->setGpioOutput(GPIO_OUTPUT_PUSHPULL);
        m_gpioWidget[i]->setGpioSpeed(GPIO_SPEED_LOW);
        m_gpioWidget[i]->setGpioVal(GPIO_RESET);
        m_gpioWidget[i]->setActive(false);
      }
      m_widgetsActive = false;

      m_pollTimer.stop();
      startAct->setEnabled(true);
      stopAct->setEnabled(false);
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

bool MainWindow::changeConfig()
{
  Brg_GpioConfT confs[4];
  for (int i : {0, 1, 2, 3}) {
    confs[i].Mode = m_gpioWidget[i]->getGpioMode();
    confs[i].Pull = m_gpioWidget[i]->getGpioPull();
    confs[i].Speed = m_gpioWidget[i]->getGpioSpeed();
    confs[i].OutputType = m_gpioWidget[i]->getGpioOutput();
  }
  Brg_GpioInitT init;
  init.ConfigNb = BRG_GPIO_MAX_NB;
  init.GpioMask = BRG_GPIO_ALL;
  init.pGpioConf = confs;
  auto ret = m_brg->InitGPIO(&init);
  if (ret != BRG_NO_ERR) {
    statusBar()->showMessage(tr("GPIO init failed with error: %1").arg(ret));
    return false;
  }
  return true;
}

bool MainWindow::changeValue(Brg_GpioMaskT mask)
{
  Brg_GpioValT gpioValues[4];
  uint8_t errorMask;

  for (int i : {0, 1, 2, 3}) {
    gpioValues[i] = m_gpioWidget[i]->getGpioVal();
  }

  auto ret = m_brg->SetResetGPIO(mask, gpioValues, &errorMask);
  if (ret != BRG_NO_ERR) {
    statusBar()->showMessage(tr("GPIO write failed with error: %1").arg(ret));
    return false;
  } else {
    return readValues(); // read back the values and update the widgets
  }
}

bool MainWindow::readValues()
{
  Brg_GpioValT gpioValues[4];
  uint8_t errorMask;

  auto ret = m_brg->ReadGPIO(BRG_GPIO_ALL, gpioValues, &errorMask);
  if (ret != BRG_NO_ERR) {
    statusBar()->showMessage(tr("GPIO read failed with error: %1").arg(ret));
    return false;
  } else {
    for (int i : {0, 1, 2, 3}) {
      m_gpioWidget[i]->setGpioVal(gpioValues[i]);
    }
  }

  return true;
}
