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
#include "ScriptFunctions.h"

#include <QtDebug>

ScriptLaunchButtonDialog::ScriptLaunchButtonDialog (QWidget *p, QString script, QString icon, bool use, int row, int col, QString text) : Dialog (p)
{
  _keySize = "script_launch_button_dialog_window_size";
  _keyPos = "script_launch_button_dialog_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Script Launch Button Settings"), QString()));

  createMainPage();

  loadSettings();
  
  setGUI(script, icon, use, row, col, text);
  
  buttonStatus();
}

void ScriptLaunchButtonDialog::createMainPage ()
{
  // script
  ScriptFunctions sf;
  QStringList tl;
  sf.names(tl);
  
  _script = new QComboBox(this);
  connect(_script, SIGNAL(activated(int)), this, SLOT(buttonStatus()));
  _script->addItems(tl);
  _form->addRow(tr("Script"), _script);
  
  // row
  _row = new QSpinBox;
  _row->setRange(0, 10);
  _form->addRow(tr("Row"), _row);
  
  // col
  _col = new QSpinBox;
  _col->setRange(0, 10);
  _form->addRow(tr("Column"), _col);

  // text
  _text = new WidgetLineEdit(this);
  connect(_text, SIGNAL(signalStatus(bool)), this, SLOT(buttonStatus()));
  _form->addRow(tr("Text"), _text);
  
  // icon widgets
  QWidget *w = new QWidget;
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(5);
  hbox->setMargin(0);
  w->setLayout(hbox);

  // use icon checkbox
  _useIcon = new QCheckBox;
  hbox->addWidget(_useIcon);
  connect(_useIcon, SIGNAL(toggled(bool)), this, SLOT(useIconToggled(bool)));

  // icon
  _icon = new IconButton(this, QString());
  hbox->addWidget(_icon, 1, 0);

  _form->addRow(tr("Icon"), w);
}

void ScriptLaunchButtonDialog::done ()
{
  emit signalDone(_script->currentText(), _icon->file(), _useIcon->isChecked(), _row->value(), _col->value(), _text->text());
  saveSettings();
  accept();
}

void ScriptLaunchButtonDialog::useIconToggled (bool d)
{
  _icon->setEnabled(d);
}

void ScriptLaunchButtonDialog::setGUI (QString script, QString icon, bool use, int row, int col, QString text)
{
  _script->setCurrentIndex(_script->findText(script));

  _icon->setFile(icon);

  _useIcon->setChecked(use);
  useIconToggled(use);
  
  _row->setValue(row);
  
  _col->setValue(col);
  
  _text->setText(text);
}

void ScriptLaunchButtonDialog::buttonStatus ()
{
  bool status = FALSE;
  int count = 0;
  
  if (! _script->currentText().isEmpty())
    count++;

  if (! _text->text().isEmpty())
    count++;
  
  if (count == 2)
    status = TRUE;
  
  _okButton->setEnabled(status);
}
