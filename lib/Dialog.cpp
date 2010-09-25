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

#include <QtDebug>
#include <QPushButton>
#include <QMessageBox>

Dialog::Dialog ()
{
  _confirmFlag = _ConfirmNone;
  
  createGUI();
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

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  hbox->setMargin(0);
  _vbox->addLayout(hbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setSpacing(0);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);

  _icon = new QLabel;
  tvbox->addWidget(_icon);
  _icon->hide();

  tvbox->addStretch(1);
  
  tvbox = new QVBoxLayout;
  tvbox->setSpacing(5);
  tvbox->setMargin(0);
  hbox->addLayout(tvbox);

  _message = new QLabel;
  tvbox->addWidget(_message);
  _message->hide();

  _confirm = new QCheckBox;
  connect(_confirm, SIGNAL(stateChanged(int)), this, SLOT(confirmChanged(int)));
  tvbox->addWidget(_confirm);
  _confirm->hide();

  tvbox->addStretch(1);

  hbox->addStretch(1);
  
  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  _vbox->addWidget(_buttonBox);

  // set the default button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Ok);
  b->setDefault(TRUE);

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void Dialog::done ()
{
  switch (_confirmFlag)
  {
    case _ConfirmNo:
      reject();
      break;
    case _ConfirmYes:
      accept();
      break;
    default:
      accept();
      break;
  }
}

void Dialog::setMessage (QString message)
{
  setMessage2(message);

  QMessageBox tbox;
  tbox.setIcon(QMessageBox::Warning);
  _icon->setPixmap(tbox.iconPixmap());
  _icon->show();
}

void Dialog::setConfirm (QString message, QString check)
{
  setMessage2(message);

  _confirm->setText(check);
  _confirm->show();
  
  _confirmFlag = _ConfirmNo;

  QMessageBox tbox;
  tbox.setIcon(QMessageBox::Question);
  _icon->setPixmap(tbox.iconPixmap());
  _icon->show();
}

void Dialog::unsetConfirm ()
{
  _message->hide();
  
  _confirm->hide();
  _confirm->setChecked(FALSE);

  _confirmFlag = _ConfirmNone;

  _icon->hide();
}

void Dialog::confirmChanged (int state)
{
  _confirmFlag = (ConfirmStatus) state;
}

void Dialog::setIcon (QPixmap pic)
{
  _icon->setPixmap(pic);
}

void Dialog::setMessage2 (QString message)
{
  setFontBold(1);
  _message->setText(message);
  _message->show();
}

void Dialog::setFontBold (int d)
{
  QFont f = _message->font();
  f.setBold(d);
  _message->setFont(f);
}

