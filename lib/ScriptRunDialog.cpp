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
#include "ScriptFunctions.h"

#include <QtDebug>

ScriptRunDialog::ScriptRunDialog (QWidget *p, QString id, Entity settings) : Dialog (p)
{
  _newFlag = 0;
  _id = id;
  _settings = settings;
  _keySize = "script_run_dialog_window_size";
  _keyPos = "script_run_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Script"), QString()));

  createGUI();
  
  setGUI();
  
  if (_newFlag)
    _name->setFocus();
  else
    _comment->setFocus();

  loadSettings();
  
  buttonStatus();
}

void ScriptRunDialog::createGUI ()
{
  _name = new WidgetLineEdit(this);
  connect(_name, SIGNAL(signalStatus(bool)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Name"), _name);
  
  _command = new WidgetLineEdit(this);
  connect(_command, SIGNAL(signalStatus(bool)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Command"), _command);
  
  _file = new FileButton(this);
  connect(_file, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  _form->addRow(tr("Script File"), _file);

  _interval = new QSpinBox;
  _interval->setValue(60);
//  _interval->setInterval(60);
  _interval->setMinimum(0);
  _form->addRow(tr("Interval"), _interval);

  _startup = new QCheckBox;
  _form->addRow(tr("Launch when app starts"), _startup);
  
  _comment = new QTextEdit;
  _form->addRow(tr("Comment"), _comment);
}

void ScriptRunDialog::done ()
{
  if (_newFlag)
  {
    ScriptFunctions sf;
    QStringList l;
    sf.names(l);
    if (l.indexOf(_name->text()) != -1)
    {
      _message->setText(tr("Script name already exists. Enter a unique name."));
      return;
    }
  }
  
  _saveFlag++;

  KeyScriptDataBase keys;
  Data file, command, startup, interval, name, comment;
  _settings.toData(keys.indexToString(KeyScriptDataBase::_NAME), name);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);
  
  name.set(_name->text());
  _settings.set(keys.indexToString(KeyScriptDataBase::_NAME), name);
  _settings.setName(_name->text());
  
 QStringList l = _file->files();
  QString tfile = l.at(0);
  file.set(l);
  _settings.set(keys.indexToString(KeyScriptDataBase::_FILE), file);
  
  command.set(_command->text());
  _settings.set(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  
  startup.set(_startup->isChecked());
  _settings.set(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  
  interval.set(_interval->value());
  _settings.set(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  
  comment.set(_comment->toPlainText());
  _settings.set(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);
  
  saveSettings();

//  emit signalDone(command.toString(), tfile, startup.toBool(), interval.toInteger());

  accept();
}

void ScriptRunDialog::buttonStatus ()
{
  bool status = FALSE;
  int count = 0;
  
  if (! _name->text().isEmpty())
    count++;

  if (_file->fileCount())
    count++;
  
  if (! _command->text().isEmpty())
    count++;
  
  if (count == 3)
    status = TRUE;
  
  _okButton->setEnabled(status);
}

void ScriptRunDialog::setGUI ()
{
  ScriptFunctions sf;
  QStringList l;
  sf.names(l);
  _name->addItems(l);

  KeyScriptDataBase keys;
  Data file, command, startup, interval, name, comment;
  _settings.toData(keys.indexToString(KeyScriptDataBase::_NAME), name);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_STARTUP), startup);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_INTERVAL), interval);
  _settings.toData(keys.indexToString(KeyScriptDataBase::_COMMENT), comment);

  _name->setText(name.toString());
  if (name.toString().isEmpty())
    _newFlag++;
  else
    _name->setEnabled(FALSE);
  
  l = file.toList();
  if (l.size())
    _file->setFiles(l);
    
  _command->setText(command.toString());
    
  _startup->setChecked(startup.toBool());

  _interval->setValue(interval.toInteger());
  
  _comment->append(comment.toString());
}
