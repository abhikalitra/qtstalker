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

#include "ScriptRunDialog.h"
#include "WindowTitle.h"
#include "KeyScriptDataBase.h"

#include <QtDebug>

ScriptRunDialog::ScriptRunDialog (QWidget *p, QString id, Entity settings) : Dialog (p)
{
  _id = id;
  _settings = settings;
  _keySize = "script_run_dialog_window_size";
  _keyPos = "script_run_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Script"), QString()));

  createGUI();
  
  setGUI();

  loadSettings();
  
  buttonStatus();
}

void ScriptRunDialog::createGUI ()
{
  _file = new FileButton(this);
  connect(_file, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  _form->addRow(tr("Script File"), _file);

  _text = new WidgetLineEdit(this);
  connect(_text, SIGNAL(signalStatus(bool)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Command"), _text);

  _interval = new QSpinBox;
  _interval->setMinimum(0);
  _interval->setValue(0);
  _form->addRow(tr("Launch every X seconds"), _interval);

  _startup = new QCheckBox;
  _form->addRow(tr("Launch at application start"), _startup);
}

void ScriptRunDialog::done ()
{
  _saveFlag++;

  KeyScriptDataBase keys;
  Data file, command, startup, interval;
  _settings.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  QStringList l = _file->files();
  QString tfile = l.at(0);
  file.set(l);
  _settings.set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  
  command.set(_text->text());
  _settings.set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  
  startup.set(_startup->isChecked());
  _settings.set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  
  interval.set(_interval->value());
  _settings.set(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  saveSettings();

  emit signalDone(command.toString(), tfile, startup.toBool(), interval.toInteger());

  accept();
}

void ScriptRunDialog::buttonStatus ()
{
  bool status = FALSE;
  int count = 0;
  
  if (_file->fileCount())
    count++;
  
  if (! _text->text().isEmpty())
    count++;
  
  if (count == 2)
    status = TRUE;
  
  _okButton->setEnabled(status);
}

void ScriptRunDialog::setGUI ()
{
  KeyScriptDataBase keys;
  Data file, command, startup, interval;
  _settings.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), interval);
  
  QStringList l = file.toList();
  if (l.size())
    _file->setFiles(l);
    
  _text->setText(command.toString());
    
  _startup->setChecked(startup.toBool());
    
  _interval->setValue(interval.toInteger());
}
