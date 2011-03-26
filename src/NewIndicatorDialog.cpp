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
#include "IndicatorDataBase.h"
#include "PluginFactory.h"

#include <QtDebug>
#include <QLineEdit>

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
  _name = new QComboBox;
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
  QString name = _name->lineEdit()->text();
  if (_items.contains(name))
  {
    _message->setText(name + " " + tr("already exists"));
    return;
  }

  PluginFactory fac;
  Plugin *plug = fac.plugin(_indicator->currentText());
  if (! plug)
  {
    qDebug() << "NewIndicatorDialog::done: no plugin" << _indicator->currentText();
    return;
  }

  Indicator i(0);
  plug->defaults(i.settings());
  i.setName(name);
  if (i.save())
  {
    qDebug() << "NewIndicatorDialog::done: indicator save error";
    delete plug;
    return;
  }
  delete plug;

  saveSettings();

  emit signalDone(name);

  accept();
}

void NewIndicatorDialog::loadSettings ()
{
  Dialog::loadSettings();

  IndicatorDataBase db;
  QStringList l;
  db.plugins(l);
  _indicator->addItems(l);

  db.indicators(_items);
  _name->clear();
  _name->addItems(_items);
  _name->clearEditText();
}

