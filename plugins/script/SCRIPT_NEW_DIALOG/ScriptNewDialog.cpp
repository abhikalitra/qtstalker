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

#include "ScriptNewDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "Command.h"
#include "ScriptPluginFactory.h"

#include <QtDebug>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QLayout>
#include <QSettings>

ScriptNewDialog::ScriptNewDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Script"));

  createGUI();

  loadScripts();

  loadSettings();

  nameChanged(QString());
}

void ScriptNewDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QLabel *label = new QLabel(tr("Enter name of new script:"));
  vbox->addWidget(label);

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

void ScriptNewDialog::nameChanged (const QString &text)
{
  int status = 0;
  if (text.length())
    status = 1;

  _okButton->setEnabled(status);
}

void ScriptNewDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ScriptNewDialog::done ()
{
  if (_scripts.contains(_name->lineEdit()->text()))
  {
    _message->setText(tr("Duplicate scripts name entered."));
    return;
  }
  
  QStringList l;
  l << "SCRIPT_DATABASE" << "SAVE" << _name->lineEdit()->text();

  Command command(l.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ScriptsNewDialog::done: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  QString s = _name->lineEdit()->text();
  s.remove(",");
  emit signalDone(s);

  saveSettings();

  accept();
}

void ScriptNewDialog::cancel ()
{
  saveSettings();
  reject();
}

QString ScriptNewDialog::stringData ()
{
  return _stringData;
}

void ScriptNewDialog::loadScripts ()
{
  QStringList cl;
  cl << "SCRIPT_DATABASE" << "SCRIPTS";

  Command command(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupNewDialog::loadScripts: no plugin" << command.plugin();
    return;
  }

  plug->command(command);

  _scripts = command.stringData().split(",", QString::SkipEmptyParts);
  
  _name->clear();
  _name->addItems(_scripts);
  _name->clearEditText();
}

void ScriptNewDialog::loadSettings ()
{
  QSettings settings;
  settings.beginGroup("main"); // global setting

  QSize sz = settings.value("script_new_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("script_new_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);
}

void ScriptNewDialog::saveSettings ()
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("script_new_dialog_window_size", size());
  settings.setValue("script_new_dialog_window_position", pos());
  settings.sync();
}
