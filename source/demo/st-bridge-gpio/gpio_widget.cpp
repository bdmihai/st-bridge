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

#include "stable.h"
#include "settings.h"
#include "gpio_widget.h"
#include "bridge.h"

GpioWidget::GpioWidget(const QString &name, Brg_GpioMaskT mask, QWidget *parent) : QWidget(parent)
{
  m_name = name;
  m_gpioMask = mask;

  m_gpioMode = GPIO_MODE_INPUT;
  m_gpioPull = GPIO_NO_PULL;
  m_gpioOutput = GPIO_OUTPUT_PUSHPULL;
  m_gpioSpeed = GPIO_SPEED_LOW;
  m_gpioVal = GPIO_RESET;

  setActive(false);
  setMinimumWidth(300);
  setMinimumHeight(400);
}

void GpioWidget::setActive(bool enabled)
{
  setEnabled(enabled);
}

void GpioWidget::setGpioVal(Brg_GpioValT value)
{
  bool changed = (m_gpioVal != value);

  if (changed) {
    m_gpioVal = value;
    if (isEnabled()) update();
  }
}

void GpioWidget::setGpioMode(Brg_GpioModeT value)
{
  bool changed = (m_gpioMode != value);

  if (changed) {
    m_gpioMode = value;
    if (isEnabled()) update();
  }
}

void GpioWidget::setGpioPull(Brg_GpioPullT value)
{
  bool changed = (m_gpioPull != value);

  if (changed) {
    m_gpioPull = value;
    if (isEnabled()) update();
  }
}

void GpioWidget::setGpioOutput(Brg_GpioOutputT value)
{
  bool changed = (m_gpioOutput != value);

  if (changed) {
    m_gpioOutput = value;
    if (isEnabled()) update();
  }
}

void GpioWidget::setGpioSpeed(Brg_GpioSpeedT value)
{
  bool changed = (m_gpioSpeed != value);

  if (changed) {
    m_gpioSpeed = value;
    if (isEnabled()) update();
  }
}

GpioWidget::~GpioWidget()
{
}

void GpioWidget::paintEvent(QPaintEvent *event)
{
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  drawBorder(painter, m_borderRect);
  drawModeButton(painter, m_inputModeRect, "Input", m_gpioMode == GPIO_MODE_INPUT);
  drawModeButton(painter, m_outputModeRect, "Output", m_gpioMode == GPIO_MODE_OUTPUT);

  if (m_gpioMode == GPIO_MODE_INPUT) {
    drawInputControl(painter, m_controlRect);
  } else {
    drawOutputControl(painter, m_controlRect);
  }
}

void GpioWidget::mouseReleaseEvent(QMouseEvent *event)
{
  bool changed = false;
  if (event->button() == Qt::LeftButton) {
    if (m_inputModeRect.contains(event->pos())) {
      changed = (m_gpioMode != GPIO_MODE_INPUT);
      m_gpioMode = GPIO_MODE_INPUT;
    }
    if (m_outputModeRect.contains(event->pos())) {
      changed = (m_gpioMode != GPIO_MODE_OUTPUT);
      m_gpioMode = GPIO_MODE_OUTPUT;
    }

    if (m_gpioMode == GPIO_MODE_INPUT) {
      if (m_pullupRect.contains(event->pos())) {
        Brg_GpioPullT old = m_gpioPull;
        m_gpioPull = (m_gpioPull == GPIO_PULL_UP) ? GPIO_NO_PULL : GPIO_PULL_UP;
        changed = (m_gpioPull != old);
      }
      if (m_pulldownRect.contains(event->pos())) {
        Brg_GpioPullT old = m_gpioPull;
        m_gpioPull = (m_gpioPull == GPIO_PULL_DOWN) ? GPIO_NO_PULL : GPIO_PULL_DOWN;
        changed = (m_gpioPull != old);
      }
    }

    if (m_gpioMode == GPIO_MODE_OUTPUT) {
      if (m_topFetRect.contains(event->pos())) {
        Brg_GpioOutputT old = m_gpioOutput;
        m_gpioOutput = (m_gpioOutput == GPIO_OUTPUT_PUSHPULL) ? GPIO_OUTPUT_OPENDRAIN : GPIO_OUTPUT_PUSHPULL;
        changed = (m_gpioOutput != old);
      }
      if (m_valueLowRect.contains(event->pos())) {
        m_gpioVal = GPIO_RESET;
        emit valueChanged(m_gpioMask);
        update();
      }
      if (m_valueHighRect.contains(event->pos())) {
        m_gpioVal = GPIO_SET;
        emit valueChanged(m_gpioMask);
        update();
      }
      if (m_lowRect.contains(event->pos())) {
        changed = (m_gpioSpeed != GPIO_SPEED_LOW);
        m_gpioSpeed = GPIO_SPEED_LOW;
      }
      if (m_mediumRect.contains(event->pos())) {
        changed = (m_gpioSpeed != GPIO_SPEED_MEDIUM);
        m_gpioSpeed = GPIO_SPEED_MEDIUM;
      }
      if (m_highRect.contains(event->pos())) {
        changed = (m_gpioSpeed != GPIO_SPEED_HIGH);
        m_gpioSpeed = GPIO_SPEED_HIGH;
      }
      if (m_veryHighRect.contains(event->pos())) {
        changed = (m_gpioSpeed != GPIO_SPEED_VERY_HIGH);
        m_gpioSpeed = GPIO_SPEED_VERY_HIGH;
      }
    }
  }

  if (changed) {
    emit configChanged();
    update();
  }
}

void GpioWidget::resizeEvent(QResizeEvent *event)
{
  Q_UNUSED(event);

  qreal x = rect().width() / 256.0;
  qreal y = rect().height() / 256.0;

  m_borderRect = QRectF(0, 0, 256.0*x, 256.0*y).adjusted(1, 1, -1, -1);
  m_inputModeRect = QRectF(5, 5, 128*x - 5, 50);
  m_outputModeRect = QRectF(128*x, 5, 128*x - 5, 50);
  m_controlRect = QRectF(5, 50 + 10, 256*x - 10, 256*y - 50 - 15);
  m_pullupRect = QRectF(256*x - 100, 50 + 60, 20, 80);
  m_pulldownRect = QRectF(256*x - 100, 256*y - 140, 20, 80);
  m_topFetRect = QRectF(256*x - 100, 50 + 60, 50, 50);
  m_bottomFetRect = QRectF(256*x - 100, 256*y - 120, 50, 50);
  m_valueLowRect = QRectF(20, 108*y + 55, 40, 30);
  m_valueHighRect = QRectF(20, 108*y + 25, 40, 30);
  m_lowRect      = QRectF(20,  80, 10, 10);
  m_mediumRect   = QRectF(20, 100, 10, 10);
  m_highRect     = QRectF(20, 120, 10, 10);
  m_veryHighRect = QRectF(20, 140, 10, 10);
}

void GpioWidget::drawBorder(QPainter &painter, QRectF &rect)
{
  painter.setClipRect(rect);

  painter.setPen(QPen(palette().dark(), 1));
  painter.setBrush(palette().window());
  painter.drawRect(rect);

  painter.setPen(QPen(palette().text(), 1));
  painter.setFont(QFont("Helvetica Bold", 11, QFont::Bold));
  painter.drawText(QRectF(rect.bottomLeft().x() + 10, rect.bottomLeft().y() - 25, 80, 20), m_name, Qt::AlignLeft | Qt::AlignBottom);
}

void GpioWidget::drawModeButton(QPainter &painter, QRectF &rect, QString text, bool active)
{
  painter.setClipRect(rect);

  painter.setPen(QPen(palette().dark(), 1));
  painter.setBrush(active ? palette().highlight() : palette().window());
  painter.drawRect(rect);

  painter.setPen(QPen(palette().text(), 1));
  painter.setFont(QFont("Helvetica Bold", 11, QFont::Bold));
  painter.drawText(rect, text, Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawInputControl(QPainter &painter, QRectF &rect)
{
  painter.setClipRect(rect);

  drawPullRezistors(painter, m_pullupRect, m_pulldownRect);
  drawIOLevel(painter, m_valueHighRect, m_valueLowRect, false);

  painter.drawLine(m_valueLowRect.topRight().x(),
                   m_valueLowRect.topRight().y()+1,
                   rect.topRight().x() - 40,
                   m_valueLowRect.topRight().y());

  painter.setBrush(palette().window());
  painter.drawEllipse(QRectF(rect.topRight().x() - 40, m_valueLowRect.topRight().y() - 5, 9, 9));
  painter.drawText(QRectF(rect.topRight().x() - 50, m_valueLowRect.topRight().y() - 30, 40, 20), "Input", Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawOutputControl(QPainter &painter, QRectF &rect)
{
  painter.setClipRect(rect);

  drawFets(painter, m_topFetRect, m_bottomFetRect);
  drawIOLevel(painter, m_valueHighRect, m_valueLowRect, true);
  drawIOSpeed(painter, m_lowRect, m_mediumRect, m_highRect, m_veryHighRect);

  painter.setPen(QPen(palette().dark(), 1));
  painter.setBrush(palette().window());

  painter.drawLine(rect.topRight().x() - 120,
                   m_topFetRect.topLeft().y() + m_topFetRect.height() / 2,
                   rect.topRight().x() - 120,
                   m_bottomFetRect.bottomLeft().y() - m_bottomFetRect.height() / 2);

  painter.drawLine(rect.topRight().x() - 120,
                   m_topFetRect.topLeft().y() + m_topFetRect.height() / 2,
                   m_topFetRect.topLeft().x(),
                   m_topFetRect.topLeft().y() + m_topFetRect.height() / 2);

  painter.drawLine(rect.topRight().x() - 120,
                   m_bottomFetRect.bottomLeft().y() - m_bottomFetRect.height() / 2,
                   m_bottomFetRect.topLeft().x() + 15,
                   m_bottomFetRect.bottomLeft().y() - m_bottomFetRect.height() / 2);

  painter.drawLine(m_valueLowRect.topRight().x(),
                   m_valueLowRect.topRight().y()+1,
                   rect.topRight().x() - 120,
                   m_valueLowRect.topRight().y());

  painter.drawLine(m_bottomFetRect.topRight().x() - 20,
                   m_valueLowRect.topRight().y()+1,
                   rect.topRight().x() - 40,
                   m_valueLowRect.topRight().y());

  painter.drawEllipse(QRectF(rect.topRight().x() - 40, m_valueLowRect.topRight().y() - 5, 9, 9));
  painter.drawText(QRectF(rect.topRight().x() - 50, m_valueLowRect.topRight().y() - 30, 40, 20), "Output", Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawPullRezistors(QPainter &painter, QRectF &pullupRect, QRectF &pulldownRect)
{
  painter.setFont(QFont("Helvetica Bold", 9));

  painter.setPen(QPen(palette().dark(), 1));
  painter.setBrush((m_gpioPull == GPIO_PULL_UP) ? palette().highlight() : palette().window());
  painter.drawRect(pullupRect);
  painter.setBrush((m_gpioPull == GPIO_PULL_DOWN) ? palette().highlight() : palette().window());
  painter.drawRect(pulldownRect);

  // vertical line betwen rezistors
  painter.drawLine(pullupRect.bottomLeft().x() + pullupRect.width()/2,
                   pullupRect.bottomLeft().y(),
                   pulldownRect.topLeft().x() + pulldownRect.width()/2,
                   pulldownRect.topLeft().y());

  // top connection to 3.3V
  painter.drawLine(pullupRect.topLeft().x() + pullupRect.width()/2,
                   pullupRect.topLeft().y(),
                   pullupRect.topLeft().x() + pullupRect.width()/2,
                   pullupRect.topLeft().y() - 20);
  painter.drawLine((pullupRect.topLeft().x() + pullupRect.topRight().x()) / 2 - 10,
                   pullupRect.topLeft().y() - 20,
                   (pullupRect.topLeft().x() + pullupRect.topRight().x()) / 2 + 10,
                   pullupRect.topLeft().y() - 20);
  painter.drawText(QRectF(pullupRect.topLeft().x() - 20,
                          pullupRect.topLeft().y() - 45,
                          pullupRect.topRight().x() - pullupRect.topLeft().x() + 40,
                          20), "+3V3", Qt::AlignHCenter | Qt::AlignVCenter);

  // bottom connection to GND
  painter.drawLine(pulldownRect.bottomLeft().x() + pulldownRect.width()/2,
                   pulldownRect.bottomLeft().y(),
                   pulldownRect.bottomLeft().x() + pulldownRect.width()/2,
                   pulldownRect.bottomLeft().y() + 20);
  painter.drawLine((pulldownRect.bottomLeft().x() + pulldownRect.bottomRight().x()) / 2 - 10,
                   pulldownRect.bottomLeft().y() + 20,
                   (pulldownRect.bottomLeft().x() + pulldownRect.bottomRight().x()) / 2 + 10,
                   pulldownRect.bottomLeft().y() + 20);
  painter.drawText(QRectF(pulldownRect.bottomLeft().x() - 20,
                          pulldownRect.bottomLeft().y() + 25,
                          pulldownRect.bottomRight().x() - pulldownRect.bottomLeft().x() + 40,
                          20), "GND", Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawFets(QPainter &painter, QRectF &topFetRect, QRectF &bottomFetRect)
{
  painter.setPen(QPen(palette().dark(), 1));

  painter.setBrush((m_gpioOutput == GPIO_OUTPUT_PUSHPULL) ? palette().highlight() : palette().window());
  painter.drawEllipse(topFetRect);
  painter.setBrush(palette().window());
  painter.drawEllipse(bottomFetRect);
  painter.drawLine(bottomFetRect.topLeft().x() + 15,
                   bottomFetRect.topLeft().y() + 10,
                   bottomFetRect.topLeft().x() + 15,
                   bottomFetRect.bottomLeft().y() - 10);
  painter.drawLine(bottomFetRect.topLeft().x() + 20,
                   bottomFetRect.topLeft().y() + 10,
                   bottomFetRect.topLeft().x() + 20,
                   bottomFetRect.bottomLeft().y() - 10);
  painter.drawLine(bottomFetRect.topLeft().x() + 20,
                   bottomFetRect.topLeft().y() + 10,
                   bottomFetRect.topRight().x() - 20,
                   bottomFetRect.topLeft().y() + 10);
  painter.drawLine(bottomFetRect.topLeft().x() + 20,
                   bottomFetRect.bottomLeft().y() - 10,
                   bottomFetRect.topRight().x() - 20,
                   bottomFetRect.bottomLeft().y() - 10);

  painter.setFont(QFont("Helvetica Bold", 9));
  painter.setPen(QPen(palette().text(), 1));
  painter.drawText(topFetRect, (m_gpioOutput == GPIO_OUTPUT_PUSHPULL) ? "Push\nPull" : "Open\nDrain", Qt::AlignHCenter | Qt::AlignVCenter);

  painter.setPen(QPen(palette().dark(), 1));

  // vertical line betwen fets
  painter.drawLine(topFetRect.bottomRight().x() - 20,
                   topFetRect.bottomLeft().y(),
                   bottomFetRect.topRight().x() - 20,
                   bottomFetRect.topLeft().y() + 10);

  // top connection to 3.3V
  painter.drawLine(topFetRect.bottomRight().x() - 20,
                   topFetRect.topLeft().y(),
                   topFetRect.bottomRight().x() - 20,
                   topFetRect.topLeft().y() - 20);
  painter.drawLine(topFetRect.bottomRight().x() - 30,
                   topFetRect.topLeft().y() - 20,
                   topFetRect.bottomRight().x() - 10,
                   topFetRect.topLeft().y() - 20);
  painter.drawText(QRectF(topFetRect.topLeft().x() - 15,
                          topFetRect.topLeft().y() - 45,
                          topFetRect.topRight().x() - topFetRect.topLeft().x() + 40,
                          20), "+3V3", Qt::AlignHCenter | Qt::AlignVCenter);

  // bottom connection to GND
  painter.drawLine(bottomFetRect.bottomRight().x() - 20,
                   bottomFetRect.bottomLeft().y() - 10,
                   bottomFetRect.bottomRight().x() - 20,
                   bottomFetRect.bottomLeft().y() + 20);
  painter.drawLine(bottomFetRect.bottomRight().x() - 30,
                   bottomFetRect.bottomLeft().y() + 20,
                   bottomFetRect.bottomRight().x() - 10,
                   bottomFetRect.bottomLeft().y() + 20);
  painter.drawText(QRectF(bottomFetRect.bottomLeft().x() - 15,
                          bottomFetRect.bottomLeft().y() + 25,
                          bottomFetRect.bottomRight().x() - bottomFetRect.bottomLeft().x() + 40,
                          20), "GND", Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawIOLevel(QPainter &painter, QRectF &highRect, QRectF &lowRect, bool active)
{
  painter.setPen(QPen(palette().dark(), 1));
  painter.setFont(QFont("Helvetica Bold", 9));

  painter.setBrush((m_gpioVal == GPIO_SET) ? palette().highlight() : palette().window());
  painter.drawRect(highRect);
  painter.setBrush((m_gpioVal == GPIO_RESET) ? palette().highlight() : palette().window());
  painter.drawRect(lowRect);

  painter.setPen(active ? QPen(palette().text(), 1) : QPen(palette().dark(), 1));
  painter.drawText(highRect, "High", Qt::AlignHCenter | Qt::AlignVCenter);
  painter.drawText(lowRect, "Low", Qt::AlignHCenter | Qt::AlignVCenter);
}

void GpioWidget::drawIOSpeed(QPainter &painter, QRectF &lowRect, QRectF &mediumRect, QRectF &highRect, QRectF &veryHighRect)
{
  painter.setFont(QFont("Helvetica Bold", 9));
  painter.setPen(QPen(palette().dark(), 1));

  painter.setBrush((m_gpioSpeed == GPIO_SPEED_LOW) ? palette().highlight() : palette().window());
  painter.drawEllipse(lowRect);
  painter.setBrush((m_gpioSpeed == GPIO_SPEED_MEDIUM) ? palette().highlight() : palette().window());
  painter.drawEllipse(mediumRect);
  painter.setBrush((m_gpioSpeed == GPIO_SPEED_HIGH) ? palette().highlight() : palette().window());
  painter.drawEllipse(highRect);
  painter.setBrush((m_gpioSpeed == GPIO_SPEED_VERY_HIGH) ? palette().highlight() : palette().window());
  painter.drawEllipse(veryHighRect);

  painter.setPen(QPen(palette().text(), 1));
  painter.drawText(lowRect.adjusted(20, -5, 70, 5), "Low", Qt::AlignLeft | Qt::AlignVCenter);
  painter.setPen(QPen(palette().text(), 1));
  painter.drawText(mediumRect.adjusted(20, -5, 70, 5), "Medium", Qt::AlignLeft | Qt::AlignVCenter);
  painter.setPen(QPen(palette().text(), 1));
  painter.drawText(highRect.adjusted(20, -5, 70, 5), "High", Qt::AlignLeft | Qt::AlignVCenter);
  painter.setPen(QPen(palette().text(), 1));
  painter.drawText(veryHighRect.adjusted(20, -5, 70, 5), "Very high", Qt::AlignLeft | Qt::AlignVCenter);
}
