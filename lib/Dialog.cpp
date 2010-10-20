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

#include "Dialog.h"
#include "Config.h"

#include "../pics/ok.xpm"
#include "../pics/disable.xpm"

#include <QtDebug>
#include <QMessageBox>

Dialog::Dialog ()
{
  _configSizeParm = -1;
  _configPosParm = -1;
  
  createGUI();

  QMessageBox tbox;
  tbox.setIcon(QMessageBox::Warning);
  _icon->setPixmap(tbox.iconPixmap());
}

Dialog::Dialog (QWidget *parent) : QDialog (parent, 0)
{
  createGUI();
}

void Dialog::createGUI ()
{
  setAttribute(Qt::WA_DeleteOnClose);

  _vbox = new QVBoxLayout;
  _vbox->setSpacing(10);
  _vbox->setMargin(5);
  setLayout(_vbox);

  _tabs = new QTabWidget;
  _vbox->addWidget(_tabs);

  // message widget
  _messageWidget = new QWidget;
  _vbox->addWidget(_messageWidget);
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  hbox->setMargin(0);
  _messageWidget->setLayout(hbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);

  _icon = new QLabel;
  tvbox->addWidget(_icon);

  tvbox->addStretch(1);
  
  tvbox = new QVBoxLayout;
  tvbox->setSpacing(10);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);

  _message = new QLabel;
  tvbox->addWidget(_message);

  QHBoxLayout *thbox = new QHBoxLayout;
  thbox->setSpacing(5);
  thbox->setMargin(0);
  tvbox->addLayout(thbox);
  
  _yesButton = new QPushButton;
  _yesButton->setText(tr("Yes"));
  _yesButton->setIcon(QIcon(ok_xpm));
  connect(_yesButton, SIGNAL(clicked()), this, SLOT(confirmYes()));
  thbox->addWidget(_yesButton);
  _yesButton->hide();

  _noButton = new QPushButton;
  _noButton->setText(tr("No"));
  _noButton->setIcon(QIcon(disable_xpm));
  connect(_noButton, SIGNAL(clicked()), this, SLOT(confirmNo()));
  thbox->addWidget(_noButton);

  tvbox->addStretch(1);

  hbox->addStretch(1);
  
  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  _vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  _messageWidget->hide();
}

void Dialog::done ()
{
  accept();
}

void Dialog::cancel ()
{
  reject();
}

void Dialog::confirmYes ()
{
  accept();
}

void Dialog::confirmNo ()
{
  reject();
}

void Dialog::setMessage (QString message)
{
  _message->setText(message);
  _messageWidget->show();
  _yesButton->hide();
  _noButton->hide();
}

void Dialog::setConfirm ()
{
  _yesButton->show();
  _noButton->show();
  
  QMessageBox tbox;
  tbox.setIcon(QMessageBox::Question);
  _icon->setPixmap(tbox.iconPixmap());
}

void Dialog::unsetConfirm ()
{
  _messageWidget->hide();
  _yesButton->hide();
  _noButton->hide();
}

void Dialog::setIcon (QPixmap pic)
{
  _icon->setPixmap(pic);
}

QFont Dialog::messageFont ()
{
  return _message->font();
}

void Dialog::setMessageFont (QFont font)
{
  _message->setFont(font);
}

void Dialog::loadSettings ()
{
  Config config;

  // restore the size of the window
  QSize sz;
  config.getData((Config::Parm) _configSizeParm, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  QPoint p;
  config.getData((Config::Parm) _configPosParm, p);
  if (! p.isNull())
    move(p);
}

void Dialog::saveSettings ()
{
  Config config;

  // save app size and position
  QSize sz = size();
  config.setData((Config::Parm) _configSizeParm, sz);

  QPoint pt = pos();
  config.setData((Config::Parm) _configPosParm, pt);
}

void Dialog::hideMessage ()
{
  _messageWidget->hide();
}

