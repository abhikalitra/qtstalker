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

#include "NewDialog.h"

#include <QtDebug>
#include <QPushButton>

NewDialog::NewDialog ()
{
  createMainPage();
  
  _name->setFocus();

  nameChanged(QString());
}

void NewDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  _ndvbox = new QVBoxLayout;
  _ndvbox->setSpacing(2);
  w->setLayout(_ndvbox);

  _name = new QComboBox;
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  _ndvbox->addWidget(_name);
  
  _tabs->addTab(w, tr("Name"));
}

void NewDialog::nameChanged (const QString &text)
{
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Ok);

  int status = 0;
  if (text.length())
    status = 1;
  
  b->setEnabled(status);
}

void NewDialog::setList (QStringList &l)
{
  _name->clear();
  _name->addItems(l);
  _name->clearEditText();
}
