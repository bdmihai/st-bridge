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

#pragma once

class GpioWidget;
class DeviceComboBox;
class STLinkInterface;
class Brg;
class MainWindow : public QMainWindow
{
    Q_OBJECT;

  public:
    MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

  protected:
    virtual void showEvent(QShowEvent *event);
    virtual void closeEvent(QCloseEvent *event);

  private slots:
    void start();
    void stop();
    void refresh();
    void readData();
    void about();
    void help();

    bool changeConfig();
    bool changeValue(Brg_GpioMaskT mask);
    bool readValues();

  private:
    void createLayout();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDockWindows();
    void readSettings();
    void writeSettings();

  private:
    bool isFirstTime;

    STLinkInterface *m_stInterface;
    Brg *m_brg;
    QTimer m_pollTimer;
    bool m_widgetsActive;

    // controls
    DeviceComboBox *m_deviceComboBox;
    GpioWidget *m_gpioWidget[4];

    // menus
    QMenu *m_fileMenu;
    QMenu *m_helpMenu;

    // toolbars
    QToolBar *m_mainToolBar;

    // actions
    QAction *startAct;
    QAction *stopAct;
    QAction *refreshAct;
    QAction *helpAct;
    QAction *aboutAct;
    QAction *exitAct;

    QProcess *m_helpProcess;
};

