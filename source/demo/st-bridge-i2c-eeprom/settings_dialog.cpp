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
#include "settings_dialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
  createLayout();
}

SettingsDialog::~SettingsDialog()
{

}

void SettingsDialog::createLayout()
{
  QVBoxLayout *mainLayout = new QVBoxLayout();
  QGridLayout *gridLayout = new QGridLayout();

  QLabel *label = new QLabel(this);
  label->setText("EEPROM I2C Address");
  gridLayout->addWidget(label, 0, 0);
  m_eepromAddress = new QLineEdit(this);
  m_eepromAddress->setAlignment(Qt::AlignRight);
  m_eepromAddress->setText(QString("%1").arg(settings.getEepromAddress(), 8, 2, QChar('0')));
  gridLayout->addWidget(m_eepromAddress, 0, 1);

  label = new QLabel(this);
  label->setText("EEPROM Size");
  gridLayout->addWidget(label, 1, 0);
  m_eepromSize = new QLineEdit(this);
  m_eepromSize->setAlignment(Qt::AlignRight);
  m_eepromSize->setText(QString("%1").arg(settings.getEepromSize()));
  gridLayout->addWidget(m_eepromSize, 1, 1);

  label = new QLabel(this);
  label->setText("EEPROM Page Size");
  gridLayout->addWidget(label, 2, 0);
  m_eepromPageSize = new QLineEdit(this);
  m_eepromPageSize->setAlignment(Qt::AlignRight);
  m_eepromPageSize->setText(QString("%1").arg(settings.getEepromPageSize()));
  gridLayout->addWidget(m_eepromPageSize, 2, 1);

  mainLayout->addLayout(gridLayout);

  // line delimitter
  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  mainLayout->addStretch();
  mainLayout->addWidget(line);

  // dialog buttons
  QDialogButtonBox *dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                           Qt::Horizontal);
  connect(dialogButtonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(dialogButtonBox, SIGNAL(rejected()), this, SLOT(reject()));
  mainLayout->addWidget(dialogButtonBox);

  setLayout(mainLayout);
  setWindowTitle("Settings");
  setWindowIcon(QIcon(":/images/settings.png"));
  setWindowFlags(windowFlags() &~ Qt::WindowContextHelpButtonHint );
  setMinimumWidth (500);
  setMinimumHeight (200);
}

void SettingsDialog::accept()
{
  settings.setEepromAddress(m_eepromAddress->text().toUInt(nullptr, 2));
  settings.setEepromSize(m_eepromSize->text().toUInt());
  settings.setEepromPageSize(m_eepromPageSize->text().toUInt());
  QDialog::accept();
}

void SettingsDialog::reject()
{
  QDialog::reject();
}
