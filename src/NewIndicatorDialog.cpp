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

#include "NewIndicatorDialog.h"
#include "Globals.h"
#include "PluginFactory.h"
#include "DataDataBase.h"

#include <QtDebug>
#include <QSettings>

NewIndicatorDialog::NewIndicatorDialog (QWidget *p) : Dialog (p)
{
  _keySize = "new_indicator_dialog_window_size";
  _keyPos = "new_indicator_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("New Indicator");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  _name->setFocus();

  nameChanged(QString());
}

void NewIndicatorDialog::createGUI ()
{
  _edit = new LineEdit(this);
  
  _name = new QComboBox;
  _name->setLineEdit(_edit);
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  _form->addRow(tr("Name"), _name);

  _indicator = new QComboBox;
  _form->addRow(tr("Indicator"), _indicator);
}

void NewIndicatorDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void NewIndicatorDialog::done ()
{
  QString name = _edit->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  Indicator i;
  i.setName(name);
  if (i.save())
  {
    qDebug() << "NewIndicatorDialog::done: indicator save error";
    return;
  }

  i.add(name);

  saveSettings();

  emit signalDone(name);

  accept();
}

void NewIndicatorDialog::loadSettings ()
{
  Dialog::loadSettings();

  QStringList l;
  QSettings set(g_globalSettings);
  l = set.value("indicator_plugins").toStringList();
  l.sort();
  _indicator->addItems(l);

  DataDataBase db("indicators");
  db.names(l);
  _items = l;

  _name->clear();
  _name->addItems(_items);
  _name->clearEditText();
}
