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

#include "GroupSelectDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QStringList>
#include <QFormLayout>
#include <QSettings>

GroupSelectDialog::GroupSelectDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("Select Group"));

  createGUI();

  loadGroups();

  loadSettings();

  buttonStatus();
}

void GroupSelectDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(0);
  vbox->addLayout(form);

  // groups
  _groups = new QComboBox;
  _groups->setToolTip(tr("Current groups"));
  form->addRow(tr("Groups"), _groups);

  // status message
  _message = new QLabel;
  vbox->addWidget(_message);

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

void GroupSelectDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void GroupSelectDialog::done ()
{
  emit signalDone(_groups->currentText());

  saveSettings();
  
  accept();
}

void GroupSelectDialog::cancel ()
{
  saveSettings();
  reject();
}

void GroupSelectDialog::buttonStatus ()
{
  int status = FALSE;
  if (_groups->count())
    status = TRUE;

  _okButton->setEnabled(status);
}

QString GroupSelectDialog::stringData ()
{
  return _groups->currentText();
}

void GroupSelectDialog::loadGroups ()
{
  QStringList cl;
  cl << "GROUP_DATABASE" << "GROUPS";

  Command command(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupSelectDialog::loadGroups: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  cl = command.stringData().split(",");

  _groups->clear();
  _groups->addItems(cl);
}

void GroupSelectDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("group_select_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("group_select_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void GroupSelectDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("group_select_dialog_window_size", size());
  settings.setValue("group_select_dialog_window_position", pos());
  settings.sync();
}
