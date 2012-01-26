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

#include "DialogNew.h"
#include "WindowTitle.h"

#include <QtDebug>

DialogNew::DialogNew (QWidget *p, QString id, Entity settings) : Dialog (p)
{
  _id = id;
  _settings = settings;
  _keySize = "new_dialog_window_size";
  _keyPos = "new_dialog_window_position";

  createGUI();
  
  if (! _id.isEmpty())
  {
    Data list;
    _settings.toData(QString("LIST"), list);
    setItems(list.toList());
  }
  
  _name->setFocus();

  loadSettings();

  nameChanged(QString());
  
  WindowTitle wt;
  setWindowTitle(wt.title(tr("New Item"), QString()));
}

void DialogNew::createGUI ()
{
  _title = new QLabel(tr("Enter new item name"));
  _vbox->insertWidget(0, _title);

  _edit = new LineEdit(this);

  _name = new QComboBox;
  _name->setLineEdit(_edit);
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  _vbox->insertWidget(1, _name);
}

void DialogNew::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void DialogNew::done ()
{
  QString name = _edit->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  _saveFlag++;
  _settings.set(QString("NAME"), Data(name));

  saveSettings();

  emit signalDone(name);

  accept();
}

void DialogNew::setItems (QStringList l)
{
  _items = l;
  _name->clear();
  _name->addItems(l);
  _name->clearEditText();
}

void DialogNew::setTitle (QString d)
{
  _title->setText(d);
}
