/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "AlertConfigureDialog.h"
#include "Globals.h"
#include "AlertConfig.h"

#include <QtDebug>

AlertConfigureDialog::AlertConfigureDialog ()
{
  _intervalFlag = 0;
  _enableFlag = 0;
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Configure Alert"));

  createMainPage();

  createMailPage();

  loadSettings();
}

void AlertConfigureDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(2);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // enable/disable
  QLabel *label = new QLabel(tr("Enable Alerts"));
  grid->addWidget(label, row, col++);

  _enable = new QCheckBox;
  _enable->setToolTip(tr("Enable / disable alerts"));
  grid->addWidget(_enable, row++, col--);

  // timer
  label = new QLabel(tr("Interval"));
  grid->addWidget(label, row, col++);
  
  _interval = new QSpinBox;
  _interval->setRange(0, 9999);
  _interval->setToolTip(tr("Scan interval in minutes"));
  grid->addWidget(_interval, row++, col--);

  // sound
  label = new QLabel(tr("Alert Sound"));
  grid->addWidget(label, row, col++);

  _sound = new FileButton(w, QString());
  _sound->setToolTip(tr("Sound file to play for notification"));
  grid->addWidget(_sound, row++, col--);

  vbox->addStretch();

  _tabs->addTab(w, tr("Settings"));
}

void AlertConfigureDialog::createMailPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // mail address
  QLabel *label = new QLabel(tr("Address"));
  grid->addWidget(label, row, col++);

  _address = new QLineEdit;
  _address->setToolTip(tr("Address to receive mail\ne.g yourname@youraddress.com"));
  grid->addWidget(_address, row++, col--);

  // mail subject
  label = new QLabel(tr("Subject"));
  grid->addWidget(label, row, col++);

  _subject = new QLineEdit;
  _subject->setToolTip(tr("Subject header of mail\ne.g. Qtstalker Alert"));
  grid->addWidget(_subject, row++, col--);

  // mail text
  label = new QLabel(tr("Message"));
  grid->addWidget(label, row, col++);

  _text = new QTextEdit;
  _text->setToolTip(tr("Message body of mail\ne.g. Qtstalker alert was triggered."));
  grid->addWidget(_text, row++, col--);

  vbox->addStretch();

  _tabs->addTab(w, tr("Email"));
}

void AlertConfigureDialog::loadSettings ()
{
  AlertConfig config;

  _enable->setChecked(config.getInt(AlertConfig::_AlertsEnable));
  connect(_enable, SIGNAL(stateChanged(int)), this, SLOT(enableChanged(int)));

  _interval->setValue(config.getInt(AlertConfig::_TimerInterval));
  connect(_interval, SIGNAL(valueChanged(int)), this, SLOT(intervalChanged(int)));

  QString s;
  config.getData(AlertConfig::_SoundFile, s);
  _sound->setFile(s);

  config.getData(AlertConfig::_MailAddress, s);
  _address->setText(s);

  config.getData(AlertConfig::_MailSubject, s);
  _subject->setText(s);

  config.getData(AlertConfig::_MailBody, s);
  _text->setText(s);
}

void AlertConfigureDialog::intervalChanged (int d)
{
  if (d != _interval->value())
    _intervalFlag = 1;
}

void AlertConfigureDialog::enableChanged (int d)
{
  if ((Qt::CheckState) d != _enable->checkState())
    _enableFlag = 1;
}

void AlertConfigureDialog::done ()
{
  AlertConfig config;
  config.transaction();

  config.setData(AlertConfig::_AlertsEnable, _enable->isChecked());

  config.setData(AlertConfig::_TimerInterval, _interval->value());

  config.setData(AlertConfig::_SoundFile, _sound->getFile());

  config.setData(AlertConfig::_MailAddress, _address->text());

  config.setData(AlertConfig::_MailSubject, _subject->text());

  config.setData(AlertConfig::_MailBody, _text->toPlainText());
  
  config.commit();

  if (_enableFlag)
    emit signalEnable((int) _enable->checkState());

  if (_intervalFlag)
    emit signalInterval(_interval->value());

  accept();
}

