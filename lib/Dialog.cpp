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

  _message = new QLabel;
  _vbox->addWidget(_message);
  _message->hide();

/*  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setMargin(0);
  grid->setColumnStretch(0, 1);
  _vbox->addLayout(grid);
*/

  _confirm = new QCheckBox;
  connect(_confirm, SIGNAL(stateChanged(int)), this, SLOT(confirmChanged(int)));
//  grid->addWidget(_confirm, 0, 1);
  _vbox->addWidget(_confirm);
  _confirm->hide();

//  _vbox->addStretch(1);

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
  _message->setText(message);
  _message->show();
}

void Dialog::setConfirm (QString message, QString check)
{
  _message->setText(message);
  _message->show();

  _confirm->setText(check);
  _confirm->show();
  
  _confirmFlag = _ConfirmNo;
}

void Dialog::unsetConfirm ()
{
  _message->hide();
  
  _confirm->hide();
  _confirm->setChecked(FALSE);

  _confirmFlag = _ConfirmNone;
}

void Dialog::confirmChanged (int state)
{
  _confirmFlag = (ConfirmStatus) state;
}

