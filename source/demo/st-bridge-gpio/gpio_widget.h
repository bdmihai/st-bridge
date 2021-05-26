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
 |  Author: Mihai Baneu                           Last modified: 09.Dec.2020  |
 |                                                                            |
 |___________________________________________________________________________*/

#pragma once

class GpioWidget : public QWidget
{
    Q_OBJECT;

  public:
    GpioWidget(const QString &name, Brg_GpioMaskT mask, QWidget *parent = nullptr);
    virtual ~GpioWidget();

    void setActive(bool enabled);

    void setGpioVal(Brg_GpioValT value);
    Brg_GpioValT getGpioVal() { return m_gpioVal; }

    void setGpioMode(Brg_GpioModeT value);
    Brg_GpioModeT getGpioMode() { return m_gpioMode; }

    void setGpioPull(Brg_GpioPullT value);
    Brg_GpioPullT getGpioPull() { return m_gpioPull; }

    void setGpioOutput(Brg_GpioOutputT value);
    Brg_GpioOutputT getGpioOutput() { return m_gpioOutput; }

    void setGpioSpeed(Brg_GpioSpeedT value);
    Brg_GpioSpeedT getGpioSpeed() { return m_gpioSpeed; }

  signals:
    void configChanged();
    void valueChanged(Brg_GpioMaskT mask);

  protected:
    void paintEvent(QPaintEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

  private:
    void drawBorder(QPainter &painter, QRectF &rect);
    void drawModeButton(QPainter &painter, QRectF &rect, QString text, bool active);
    void drawInputControl(QPainter &painter, QRectF &rect);
    void drawOutputControl(QPainter &painter, QRectF &rect);
    void drawPullRezistors(QPainter &painter, QRectF &pullupRect, QRectF &pulldownRect);
    void drawFets(QPainter &painter, QRectF &topFetRect, QRectF &bottomFetRect);
    void drawIOLevel(QPainter &painter, QRectF &highRect, QRectF &lowRect, bool active);
    void drawIOSpeed(QPainter &painter, QRectF &lowRect, QRectF &mediumRect, QRectF &highRect, QRectF &veryHighRect);

    QString m_name;
    Brg_GpioMaskT m_gpioMask;
    Brg_GpioModeT m_gpioMode;
    Brg_GpioPullT  m_gpioPull;
    Brg_GpioValT m_gpioVal;
    Brg_GpioOutputT m_gpioOutput;
    Brg_GpioSpeedT m_gpioSpeed;
    QRectF m_borderRect;
    QRectF m_inputModeRect;
    QRectF m_outputModeRect;
    QRectF m_controlRect;
    QRectF m_pullupRect;
    QRectF m_pulldownRect;
    QRectF m_topFetRect;
    QRectF m_bottomFetRect;
    QRectF m_valueLowRect;
    QRectF m_valueHighRect;
    QRectF m_lowRect;
    QRectF m_mediumRect;
    QRectF m_highRect;
    QRectF m_veryHighRect;
};
