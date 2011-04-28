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

#include "IndicatorEditDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>
#include <QDir>

IndicatorEditDialog::IndicatorEditDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "indicator_edit_dialog_window_size";
  _keyPos = "indicator_edit_dialog_window_position";

  _name = _command->parm("NAME");

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Editing Indicator") << _name;
  setWindowTitle(l.join(" "));

  createGUI();

  _indicator.setName(_name);
  _indicator.load();

/*  
  QString s = _indicator.command();
  if (! s.isEmpty())
    _com->setText(s);

  _file = _indicator.script();
  if (! _file.isEmpty())
  {
    QStringList tl;
    tl << _file;
    _fileButton->setFiles(tl);
  }
*/

  loadSettings();

  buttonStatus();

  _com->setFocus();
}

void IndicatorEditDialog::createGUI ()
{
  // command
  _com = new LineEdit(this);
  _com->setText("perl");
  _com->setToolTip(tr("Script command"));
  connect(_com, SIGNAL(textChanged(const QString &)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Command"), _com);

  // file
  _fileButton = new FileButton(this);
  connect(_fileButton, SIGNAL(signalSelectionChanged()), this, SLOT(buttonStatus()));
  _form->addRow(tr("Script File"), _fileButton);
}

void IndicatorEditDialog::buttonStatus ()
{
  int status = 0;

  QStringList l = _fileButton->files();
  if (l.count())
  {
    _file = l.at(0);
    status++;
  }

  if (_com->text().length())
    status++;

  if (status ==  2)
    _okButton->setEnabled(TRUE);
  else
    _okButton->setEnabled(FALSE);
}

void IndicatorEditDialog::done ()
{
  QString com = _com->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid script command."));
    return;
  }

//  _indicator.setCommand(com);
//  _indicator.setScript(_file);
  _indicator.save();

  _command->setReturnCode("0");

  saveSettings();

  accept();
}

void IndicatorEditDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  if (_file.isEmpty())
  {
    _file = settings.value("indicator_edit_dialog_last_file",
                           settings.value("indicator_script_directory").toString()).toString();
    QStringList l;
    l << _file;
    _fileButton->setFiles(l);
  }
}

void IndicatorEditDialog::saveSettings ()
{
  Dialog::saveSettings();

  QSettings settings(g_globalSettings);
  settings.setValue("indicator_edit_dialog_last_file", _file);
  settings.sync();
}
