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
#include "ScriptDataBase.h"
#include "Globals.h"
#include "Doc.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>
#include <QPushButton>

ScriptLaunchButtonDialog::ScriptLaunchButtonDialog (QString script, QString icon)
{
  _helpFile = "main.html";
  
  setWindowTitle("Qtstalker" + g_session + ": " +  tr("Set Date Range"));

  createMainPage();

  loadSettings();

  _script->setCurrentIndex(_script->findText(script));
  _icon->setFile(icon);

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ScriptLaunchButtonDialog::createMainPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  vbox->addLayout(form);

  // script
  ScriptDataBase db;
  QStringList l;
  db.scripts(l);

  _script = new QComboBox;
  _script->addItems(l);
  form->addRow(tr("Script"), _script);
  
  // icon
  _icon = new IconButton(this, QString());
  form->addRow(tr("Icon"), _icon);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(bbox, SIGNAL(accepted()), this, SLOT(done()));
  connect(bbox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(bbox);

  // ok button
  _okButton = bbox->addButton(QDialogButtonBox::Ok);
  _okButton->setDefault(TRUE);

  // cancel button
  _cancelButton = bbox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setDefault(TRUE);

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void ScriptLaunchButtonDialog::done ()
{
  emit signalDone(_script->currentText(), _icon->file());
  
  saveSettings();
  
  accept();
}

void ScriptLaunchButtonDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ScriptLaunchButtonDialog::cancel ()
{
  saveSettings();
  reject();
}

void ScriptLaunchButtonDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);
  QSize sz = settings.value("script_launch_button_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("script_launch_button_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void ScriptLaunchButtonDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("script_launch_button_dialog_window_size", size());
  settings.setValue("script_launch_button_dialog_window_position", pos());
  settings.sync();
}
