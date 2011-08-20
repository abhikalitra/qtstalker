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

#include "ScriptInsertDialog.h"
#include "Globals.h"
#include "CommandFactory.h"

#include <QtDebug>
#include <QSettings>
#include <QStringList>

ScriptInsertDialog::ScriptInsertDialog (QWidget *p, Script *pro) : Dialog (p)
{
  _script = pro;
  _keySize = "procedure_insert_dialog_window_size";
  _keyPos = "procedure_insert_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Insert Plugin");
  setWindowTitle(l.join(" "));

  createGUI();
}

void ScriptInsertDialog::createGUI ()
{
  // name
  _name = new QLineEdit(tr("New Step"));
  _form->addRow(tr("Step Name"), _name);

  // commands
  CommandFactory fac;
  QStringList l = fac.list();
  l.sort();

  QSettings set2(g_localSettings);
  QString s = set2.value("procedure_insert_dialog_last_indicator", 0).toString();

  _command = new QComboBox;
  _command->addItems(l);
  _command->setCurrentIndex(_command->findText(s));
  _form->addRow(tr("Command"), _command);
}

void ScriptInsertDialog::done ()
{
  if (_name->text().isEmpty())
  {
    _message->setText(tr("Step name missing"));
    return;
  }

  SettingGroup *sg = _script->settingGroup(_name->text());
  if (sg)
  {
    _message->setText(tr("Duplicate name"));
    return;
  }

  QSettings settings(g_localSettings);
  settings.setValue("procedure_insert_dialog_last_indicator", _command->currentText());
  settings.sync();

  emit signalDone(_name->text(), _command->currentText());

  accept();
}
