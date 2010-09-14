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

Dialog::Dialog (Dialog::Type type, QWidget *parent) : QDialog (parent, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);

  _vbox = new QVBoxLayout;
  _vbox->setSpacing(10);
  _vbox->setMargin(10);
  setLayout(_vbox);

  if (type == _Dialog)
  {
    _tabs = new QTabWidget;
    _vbox->addWidget(_tabs);
  }
  else
  {
    _vbox->addStretch(1);

    _message = new QLabel;
    _vbox->addWidget(_message);
  }

  _vbox->addStretch(1);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  _vbox->addWidget(_buttonBox);

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void Dialog::done ()
{
  accept();
}

void Dialog::setMessage (QString message)
{
  _message->setText(message);
}

