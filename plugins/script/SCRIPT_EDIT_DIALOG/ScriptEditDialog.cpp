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

#include "ScriptEditDialog.h"
#include "Globals.h"
#include "Doc.h"
#include "ScriptDataBase.h"

#include <QtDebug>
#include <QDialogButtonBox>
#include <QLayout>
#include <QFormLayout>
#include <QSettings>
#include <QFileDialog>

ScriptEditDialog::ScriptEditDialog (Command *c)
{
  _command = c;
  _helpFile = "cus.html";

  _name = _command->parm(1);

  QStringList l;
  l << "Qtstalker" << g_session << ":" << tr("Edit Script") << _name;
  setWindowTitle(l.join(" "));
  
  createGUI();

  loadSettings();

  loadScript();

  buttonStatus();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

void ScriptEditDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  setLayout(vbox);

  QFormLayout *form = new QFormLayout;
  form->setMargin(0);
  form->setSpacing(2);
  vbox->addLayout(form);

  // command
  _com = new QLineEdit("perl");
  _com->setToolTip(tr("Interpreter command and switches eg. perl -l -T"));
  connect(_com, SIGNAL(textEdited(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _com);
  
  // file
  _fileButton = new QPushButton;
  _fileButton->setToolTip(tr("The script location"));
  connect(_fileButton, SIGNAL(clicked()), this, SLOT(fileButtonPressed()));
  form->addRow(tr("Script File"), _fileButton);

  // startup
  _startup = new QCheckBox;
  _startup->setToolTip(tr("Run this script as soon as QtStalker starts."));
  form->addRow(tr("Run at startup"), _startup);

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

void ScriptEditDialog::buttonStatus ()
{
  int status = -1;
  if (_com->text().length())
    status++;

  if (! _file.isEmpty())
    status++;

  _okButton->setEnabled(status);
}

void ScriptEditDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void ScriptEditDialog::cancel ()
{
  saveSettings();
  reject();
}

void ScriptEditDialog::loadSettings ()
{
  QSettings settings(g_settingsFile);

  QSize sz = settings.value("script_edit_dialog_window_size", QSize(200,150)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("script_edit_dialog_window_position", QPoint(0,0)).toPoint();
  move(p);

  // last file selected
  _file = settings.value("script_edit_dialog_last_file", QDir::homePath()).toString();
  fileButtonPressed2(_file);
}

void ScriptEditDialog::saveSettings ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("script_edit_dialog_window_size", size());
  settings.setValue("script_edit_dialog_window_position", pos());
  settings.setValue("script_edit_dialog_last_file", _file);
  settings.sync();
}

void ScriptEditDialog::fileButtonPressed ()
{
  QFileDialog *dialog = new QFileDialog(this);
  QFileInfo fi(_file);
  dialog->setDirectory(fi.absolutePath() + "/");
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(fileButtonPressed2(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScriptEditDialog::fileButtonPressed2 (QString d)
{
  _file = d;

  QFileInfo fi(_file);
  _fileButton->setText(fi.fileName());

  buttonStatus();
}

void ScriptEditDialog::done ()
{
  QString com = _com->text();
  com.remove(",");
  if (com.isEmpty())
  {
    _message->setText(tr("Invalid command."));
    return;
  }

  ScriptDataBase db;
  Script script;
  script.setName(_name);
  script.setCommand(com);
  script.setFile(_file);
  script.setStartup(_startup->isChecked());
  db.save(&script);

  _command->setReturnData("0");

  saveSettings();

  accept();
}

void ScriptEditDialog::loadScript ()
{
  ScriptDataBase db;
  Script script;
  script.setName(_name);
  db.load(&script);

  _file = script.file();
  fileButtonPressed2(_file);

  _com->setText(script.command());

  _startup->setChecked(script.startup());
}
