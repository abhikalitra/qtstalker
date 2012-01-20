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

#include "ScriptLaunchButtonDialog.h"
#include "WindowTitle.h"

#include <QtDebug>

ScriptLaunchButtonDialog::ScriptLaunchButtonDialog (QWidget *p, QString command, QString script, QString icon, int use) : Dialog (p)
{
  _keySize = "script_launch_button_dialog_window_size";
  _keyPos = "script_launch_button_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Script Launch Button Settings"), QString()));

  createMainPage();

  loadSettings();

  QStringList l;
  l << script;
  _script->setFiles(l);

  _command->setText(command);

  _icon->setFile(icon);

  if (use)
  {
    _useIcon->setChecked(TRUE);
    useIconToggled(TRUE);
  }
  else
  {
    _useIcon->setChecked(FALSE);
    useIconToggled(FALSE);
  }
}

void ScriptLaunchButtonDialog::createMainPage ()
{
  // file button
  _script = new FileButton(this);
  _form->addRow(tr("Script File"), _script);

  // command
  _command = new LineEdit(this);
  _form->addRow(tr("Command"), _command);

  // icon widgets
  QWidget *w = new QWidget;
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  hbox->setMargin(0);
  w->setLayout(hbox);

  // use icon checkbox
  _useIcon = new QCheckBox;
  hbox->addWidget(_useIcon);
//  _form->addRow(tr("Use Icon"), _useIcon);
  connect(_useIcon, SIGNAL(toggled(bool)), this, SLOT(useIconToggled(bool)));

  // icon
  _icon = new IconButton(this, QString());
  hbox->addWidget(_icon, 1, 0);
//  _form->addRow(tr("Icon"), _icon);

  _form->addRow(tr("Icon"), w);
}

void ScriptLaunchButtonDialog::done ()
{
  QStringList l = _script->files();
  QString script;
  if (l.count())
    script = l.at(0);

  emit signalDone(_command->text(), script, _icon->file(), (int) _useIcon->isChecked());

  saveSettings();

  accept();
}

void ScriptLaunchButtonDialog::useIconToggled (bool d)
{
  if (d)
    _icon->setEnabled(TRUE);
  else
    _icon->setEnabled(FALSE);
}
