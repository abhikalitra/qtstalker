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
#include "ScriptDataBase.h"
#include "Globals.h"

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

  _startup = new QCheckBox;
  _form->addRow(tr("Launch at application start"), _startup);

  _interval = new QSpinBox;
  _interval->setMinimum(0);
  _form->addRow(tr("Launch every X seconds"), _interval);
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

  ScriptDataBase db;
  db.transaction();
  db.save(_name, file, startup, interval, _command->text());
  db.commit();

  saveSettings();

  emit signalDone(_name);

  accept();
}

void ScriptTimerDialog::setGUI ()
{
  ScriptDataBase db;
  QString file, startup, interval, command;
  if (db.load(_name, file, startup, interval, command))
    return;

  bool b = FALSE;
  if (startup.toInt() > 0)
    b = TRUE;
  _startup->setChecked(b);

  _interval->setValue(interval.toInt());

  QStringList l;
  l << file;
  _file->setFiles(l);

  _command->setText(command);
}
