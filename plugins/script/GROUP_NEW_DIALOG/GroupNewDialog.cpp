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

#include "GroupNewDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

GroupNewDialog::GroupNewDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Group"));

  createGUI();

  loadGroups();

  loadSettings();

  nameChanged(QString());
}

void GroupNewDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  _name = new QComboBox;
  _name->setEditable(TRUE);
  connect(_name, SIGNAL(editTextChanged(const QString &)), this, SLOT(nameChanged(const QString &)));
  vbox->addWidget(_name);

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

  // help button
  QPushButton *b = bbox->button(QDialogButtonBox::Help);
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void GroupNewDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void GroupNewDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void GroupNewDialog::done ()
{
  if (_groups.contains(_name->lineEdit()->text()))
  {
    _message->setText(tr("Duplicate group name entered."));
    return;
  }
  
  QStringList l;
  l << "GROUP_DATABASE" << "SAVE" << _name->lineEdit()->text();

  Command command(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupNewDialog::done: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  emit signalDone(_name->lineEdit()->text());

  saveSettings();

  accept();
}

void GroupNewDialog::cancel ()
{
  saveSettings();
  reject();
}

QString GroupNewDialog::stringData ()
{
  return _name->lineEdit()->text();
}

void GroupNewDialog::loadGroups ()
{
  QStringList cl;
  cl << "GROUP_DATABASE" << "GROUPS";

  Command command(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupNewDialog::loadGroups: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  _groups = command.stringData().split(",", QString::SkipEmptyParts);
  
  _name->clear();
  _name->addItems(_groups);
  _name->clearEditText();
}

void GroupNewDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("group_new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("group_new_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void GroupNewDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("group_new_dialog_window_size", size());
  settings.setValue("group_new_dialog_window_position", pos());
  settings.sync();
}
