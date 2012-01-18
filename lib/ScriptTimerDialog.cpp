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

#include "ScriptTimerDialog.h"
#include "EAVDataBase.h"
#include "Global.h"
#include "ScriptDataBaseKey.h"

#include <QtDebug>

ScriptTimerDialog::ScriptTimerDialog (QWidget *p, QString name) : Dialog (p)
{
  _name = name;
  _keySize = "script_timer_dialog_window_size";
  _keyPos = "script_timer_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit") << _name;
  setWindowTitle(l.join(" "));

  createGUI();
  loadSettings();
  setGUI();
}

void ScriptTimerDialog::createGUI ()
{
  _file = new FileButton(this);
  _form->addRow(tr("Script File"), _file);

  _command = new LineEdit(this);
  _form->addRow(tr("Command"), _command);

  _interval = new QSpinBox;
  _interval->setMinimum(0);
  _form->addRow(tr("Launch every X seconds"), _interval);

  _startup = new QCheckBox;
  _form->addRow(tr("Launch at application start"), _startup);
}

void ScriptTimerDialog::done ()
{
  QString startup = QString::number(_startup->isChecked());
  QString interval = _interval->text();

  QString file;
  QStringList l = _file->files();
  if (! l.count())
  {
    _message->setText(tr("No script file selected"));
    return;
  }
  else
    file = l.at(0);

  if (_command->text().isEmpty())
  {
    _message->setText(tr("Command missing"));
    return;
  }

  EAVDataBase db("scripts");
  db.transaction();

  ScriptDataBaseKey skeys;
  Entity data;
  data.setName(_name);
  data.set(skeys.indexToString(ScriptDataBaseKey::_FILE), Data(file));
  data.set(skeys.indexToString(ScriptDataBaseKey::_STARTUP), Data(_startup->isChecked()));
  data.set(skeys.indexToString(ScriptDataBaseKey::_RUN_INTERVAL), Data(_interval->value()));
  data.set(skeys.indexToString(ScriptDataBaseKey::_COMMAND), Data(_command->text()));
  if (db.set(data))
  {
    qDebug() << "ScriptTimerDialog::done: error saving timer" << _name;
    return;
  }

  db.commit();

  saveSettings();

  emit signalDone(_name);

  accept();
}

void ScriptTimerDialog::setGUI ()
{
  EAVDataBase db("scripts");
  Entity data;
  data.setName(_name);
  if (db.get(data))
  {
    qDebug() << "ScriptTimerDialog::setGUI: error loading timer" << _name;
    return;
  }

  ScriptDataBaseKey skeys;
  Data td;
  data.toData(skeys.indexToString(ScriptDataBaseKey::_STARTUP), td);
  _startup->setChecked(td.toBool());

  data.toData(skeys.indexToString(ScriptDataBaseKey::_RUN_INTERVAL), td);
  _interval->setValue(td.toInteger());

  QStringList l;
  data.toData(skeys.indexToString(ScriptDataBaseKey::_FILE), td);
  l << td.toString();
  _file->setFiles(l);

  data.toData(skeys.indexToString(ScriptDataBaseKey::_COMMAND), td);
  _command->setText(td.toString());
}
