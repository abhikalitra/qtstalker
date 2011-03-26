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

#include "ScriptEditDialog.h"
#include "Globals.h"
#include "ScriptDataBase.h"

#include <QtDebug>
#include <QSettings>
#include <QFileDialog>

ScriptEditDialog::ScriptEditDialog (QWidget *p, QString n) : Dialog (p)
{
  _keySize = "script_edit_dialog_window_size";
  _keyPos = "script_edit_dialog_window_position";

  _name = n;

  QStringList l;
  l << "Qtstalker" + g_session + ":" << tr("Edit Script") << _name;
  setWindowTitle(l.join(" "));
  
  createGUI();

  loadSettings();

  loadScript();

  buttonStatus();

  _com->setFocus();
}

void ScriptEditDialog::createGUI ()
{
  // command
  _com = new LineEdit;
  _com->setText("perl");
  _com->setToolTip(tr("Interpreter command and switches eg. perl -l -T"));
  connect(_com, SIGNAL(textEdited(const QString &)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Command"), _com);
  
  // file
  _fileButton = new FileButton(this);
  _fileButton->setToolTip(tr("The script location"));
  connect(_fileButton, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  _form->addRow(tr("Script File"), _fileButton);

  // minutes
  _minutes = new QSpinBox;
  _minutes->setToolTip(tr("Run this script every X minutes."));
  _form->addRow(tr("Run every X minutes"), _minutes);
}

void ScriptEditDialog::buttonStatus ()
{
  int status = -1;
  if (_com->text().length())
    status++;

  QStringList l = _fileButton->files();
  if (l.count())
  {
    _file = l.at(0);
    status++;
  }

  _okButton->setEnabled(status);
}

void ScriptEditDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  // last file selected
  _file = settings.value("script_edit_dialog_last_file", QDir::homePath()).toString();
  QStringList l;
  l << _file;
  _fileButton->setFiles(l);
}

void ScriptEditDialog::saveSettings ()
{
  Dialog::saveSettings();

  QSettings settings(g_globalSettings);
  settings.setValue("script_edit_dialog_last_file", _file);
  settings.sync();
}

void ScriptEditDialog::done ()
{
  QString com = _com->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid command."));
    return;
  }

  ScriptDataBase db;
  Script script;
  script.setName(_name);
  script.setCommand(com);
  script.setFile(_file);
  script.setMinutes(_minutes->value());
  db.save(&script);

  saveSettings();

  accept();
}

void ScriptEditDialog::loadScript ()
{
  ScriptDataBase db;
  Script script;
  script.setName(_name);
  db.load(&script);

  _file = script.file();
  QStringList l;
  l << _file;
  _fileButton->setFiles(l);

  _com->setText(script.command());

  _minutes->setValue(script.minutes());
}
