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
#include "ScriptTimerFunctions.h"

#include <QtDebug>

ScriptTimerDialog::ScriptTimerDialog (QWidget *p, QString name) : Dialog (p)
{
  _name = name;
  _keySize = "script_timer_dialog_window_size";
  _keyPos = "script_timer_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Edit Script Timer"), _name));
  
  _newFlag = FALSE;
  if (_name.isEmpty())
    _newFlag = TRUE;

  createGUI();
  loadSettings();
  setGUI();
  okButtonStatus();
}

void ScriptTimerDialog::createGUI ()
{
  _nameEdit = new WidgetLineEdit(this);
  _form->addRow(tr("Name"), _nameEdit);
  
  if (_newFlag)
  {
    ScriptTimerFunctions stf;
    QStringList l;
    stf.names(l);
    _nameEdit->setItems(l);
  }
  else
  {
    _nameEdit->setText(_name);
    _nameEdit->setEnabled(FALSE);
  }

  _file = new FileButton(this);
  _form->addRow(tr("Script File"), _file);

  _command = new WidgetLineEdit(this);
  _form->addRow(tr("Command"), _command);

  _interval = new QSpinBox;
  _interval->setMinimum(0);
  _form->addRow(tr("Launch every X seconds"), _interval);

  _startup = new QCheckBox;
  _form->addRow(tr("Launch at application start"), _startup);

  // avoid conflict from signals/slots until all objects have been created
  connect(_nameEdit, SIGNAL(signalStatus(bool)), this, SLOT(okButtonStatus()));
  connect(_file, SIGNAL(signalSelectionChanged()), this, SLOT(okButtonStatus()));
  connect(_command, SIGNAL(signalStatus(bool)), this, SLOT(okButtonStatus()));
}

void ScriptTimerDialog::done ()
{
  KeyScriptDataBase keys;
  Entity timer;
  QString name = _name;
  if (_newFlag)
    name = _nameEdit->text();
  timer.setName(name);
  
  QStringList l = _file->files();
  timer.set(keys.indexToString(KeyScriptDataBase::_FILE), Data(l.at(0)));
  
  timer.set(keys.indexToString(KeyScriptDataBase::_STARTUP), Data(_startup->isChecked()));
  timer.set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), Data(_interval->value()));
  timer.set(keys.indexToString(KeyScriptDataBase::_COMMAND), Data(_command->text()));
  
  ScriptTimerFunctions stf;
  if (stf.save(timer))
  {
    qDebug() << "ScriptTimerDialog::done: error saving timer"; 
    _message->setText(tr("Database error. Timer not saved."));
    return;
  }
  
  if (stf.add(timer))
  {
    qDebug() << "ScriptTimerDialog::done: error adding timer"; 
    _message->setText(tr("Database error. Timer not added."));
    return;
  }
  
  saveSettings();

  emit signalDone(name);

  accept();
}

void ScriptTimerDialog::setGUI ()
{
  if (_newFlag)
    return;
  
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

void ScriptTimerDialog::okButtonStatus ()
{
  int count = 0;
  bool status = FALSE;
  
  if (! _nameEdit->text().isEmpty())
    count++;
    
  if (! _command->text().isEmpty())
    count++;
  
  if (_file->fileCount())
    count++;

  if (count == 3)
    status = TRUE;
  
  _okButton->setEnabled(status);
}
