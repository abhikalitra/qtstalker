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
#include "Globals.h"

#include <QtDebug>
#include <QLineEdit>

NewDialog::NewDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "new_dialog_window_size";
  _keyPos = "new_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("New") << _command->parm("TITLE");
  setWindowTitle(l.join(" "));

  createGUI();

  _items = _command->parm("ITEMS").split(";", QString::SkipEmptyParts);

  _name->clear();
  _name->addItems(_items);
  _name->clearEditText();
  _name->setFocus();

  loadSettings();

  nameChanged(QString());
}

void NewDialog::createGUI ()
{
  QLabel *label = new QLabel(tr("Enter new") + " " + _command->parm("TITLE"));
  _vbox->insertWidget(0, label);

  _name = new QComboBox;
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  _vbox->insertWidget(1, _name);
}

void NewDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void NewDialog::done ()
{
  QString name = _name->lineEdit()->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  saveSettings();

  _command->setReturnData("NEW_DIALOG_NAME", name);

  _command->setReturnCode("0");

  accept();
}
