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
#include <QFormLayout>

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

  QFormLayout *form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  w->setLayout(form);

  // enable/disable
  _enable = new QCheckBox;
  form->addRow(tr("Enable Alerts"), _enable);

  // timer
  _interval = new QSpinBox;
  _interval->setRange(0, 9999);
  _interval->setToolTip(tr("Scan interval in minutes"));
  form->addRow(tr("Interval"), _interval);

  // sound
  _sound = new FileButton(w, QString());
  _sound->setToolTip(tr("Sound file to play for notification"));
  form->addRow(tr("Alert Sound"), _sound);

  _tabs->addTab(w, tr("Settings"));
}

void AlertConfigureDialog::createMailPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  w->setLayout(form);

  // mail address
  _address = new QLineEdit;
  _address->setToolTip(tr("Address to receive mail\ne.g yourname@youraddress.com"));
  form->addRow(tr("Address"), _address);

  // mail subject
  _subject = new QLineEdit;
  _subject->setToolTip(tr("Subject header of mail\ne.g. Qtstalker Alert"));
  form->addRow(tr("Subject"), _subject);

  // mail text
  _text = new QTextEdit;
  _text->setToolTip(tr("Message body of mail\ne.g. Qtstalker alert was triggered."));
  form->addRow(tr("Message"), _text);

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

