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

#include <QtDebug>

ScriptRunDialog::ScriptRunDialog (QWidget *p, QString file, QString command) : Dialog (p)
{
  _keySize = "script_run_dialog_window_size";
  _keyPos = "script_run_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Run Script"), QString()));

  createGUI();

  if (! file.isEmpty())
  {
    QStringList l;
    l << file;
    _file->setFiles(l);
  }

  _text->setText(command);

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
}

void ScriptRunDialog::done ()
{
  QString file;
  QStringList l = _file->files();
  if (! l.count())
  {
    _message->setText(tr("No script file selected"));
    return;
  }
  else
    file = l.at(0);

  if (_text->text().isEmpty())
  {
    _message->setText(tr("Command missing"));
    return;
  }

  saveSettings();

  emit signalDone(_text->text(), file);

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
