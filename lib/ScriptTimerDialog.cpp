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
#include "WindowTitle.h"
#include "KeyScriptDataBase.h"
#include "ScriptTimerAdd.h"

#include <QtDebug>

ScriptTimerDialog::ScriptTimerDialog (QWidget *p, QString name) : Dialog (p)
{
  _name = name;
  _keySize = "script_timer_dialog_window_size";
  _keyPos = "script_timer_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Edit Script Timer"), _name));

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

  KeyScriptDataBase keys;
  Entity timer;
  timer.setName(_name);
  timer.set(keys.indexToString(KeyScriptDataBase::_FILE), Data(file));
  timer.set(keys.indexToString(KeyScriptDataBase::_STARTUP), Data(_startup->isChecked()));
  timer.set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), Data(_interval->value()));
  timer.set(keys.indexToString(KeyScriptDataBase::_COMMAND), Data(_command->text()));
  
  ScriptTimerAdd sta;
  if (sta.add(timer))
  {
    qDebug() << "ScriptTimerDialog::done: error saving timer" << _name;
    return;
  }
  
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

  KeyScriptDataBase skeys;
  Data td;
  data.toData(skeys.indexToString(KeyScriptDataBase::_STARTUP), td);
  _startup->setChecked(td.toBool());

  data.toData(skeys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), td);
  _interval->setValue(td.toInteger());

  QStringList l;
  data.toData(skeys.indexToString(KeyScriptDataBase::_FILE), td);
  l << td.toString();
  _file->setFiles(l);

  data.toData(skeys.indexToString(KeyScriptDataBase::_COMMAND), td);
  _command->setText(td.toString());
}
