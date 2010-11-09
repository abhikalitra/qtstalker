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
#include "ScriptConfig.h"
#include "Globals.h"
#include "Strip.h"
#include "ScriptDataBase.h"
#include "ScriptItem.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDir>

ScriptEditDialog::ScriptEditDialog (QString name)
{
  _saveFlag = 0;
  _name = name;
  //_helpFile = "Script.html";
  _helpFile = "cus.html";
  
  QString s = "Qtstalker" + g_session + ": " + tr("Edit Script") + " " + _name;
  setWindowTitle(s);
  
  createMainPage();

  loadScript();

  buttonStatus();
}

void ScriptEditDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // command
  _command = new QLineEdit("perl");
  _command->setToolTip(tr("Interpreter command and switches eg. perl -l -T"));
  connect(_command, SIGNAL(textEdited(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _command);
  
  // file
  QString s;
  ScriptConfig config;
  config.getData(ScriptConfig::_LastScriptPath, s);
  if (s.isEmpty())
    s = QDir::homePath();
  
  _file = new FileButton(this, s);
  _file->setToolTip(tr("The script location"));
  connect(_file, SIGNAL(signalFileChanged()), this, SLOT(fileChanged()));
  form->addRow(tr("Script File"), _file);

  // comment
  _comment = new QTextEdit;
  connect(_comment, SIGNAL(textChanged()), this, SLOT(buttonStatus()));
  form->addRow(tr("Comment"), _comment);

  _tabs->addTab(w, tr("Settings"));
}

void ScriptEditDialog::loadScript ()
{
  ScriptDataBase db;
  ScriptItem script;
  script.setName(_name);
  db.getScript(&script);

  QString s = script.command();
  if (! s.isEmpty())
    _command->setText(s);

  s = script.file();
  if (! s.isEmpty())
    _file->setFile(s);

  s = script.comment();
  if (! s.isEmpty())
    _comment->append(s);
}

void ScriptEditDialog::buttonStatus ()
{
  bool status = TRUE;
  
  if (_command->text().isEmpty())
    status = FALSE;
  
  if (_file->getFile().isEmpty())
    status = FALSE;
  
  _okButton->setEnabled(status);

  if (status == TRUE)
    _saveFlag = 1;
}

void ScriptEditDialog::fileChanged ()
{
  ScriptConfig config;
  config.transaction();
  config.setData(ScriptConfig::_LastScriptPath, _file->getFile());
  config.commit();

  buttonStatus();
}

void ScriptEditDialog::confirmYes ()
{
  done();
}

void ScriptEditDialog::cancel ()
{
  if (_saveFlag)
  {
    setMessage(tr("Settings modified. Save changes?"));
    setConfirm();
    _okButton->setEnabled(FALSE);
    _cancelButton->setEnabled(FALSE);
  }
  else
    reject();
}

void ScriptEditDialog::done ()
{
  Strip strip;
  ScriptItem script;

  script.setName(_name);
  
  QString s = _command->text();
  strip.verifyText(s);
  script.setCommand(s);

  script.setFile(_file->getFile());
    
  s = _comment->toPlainText();
  strip.verifyText(s);
  script.setComment(s);

  ScriptDataBase db;
  db.transaction();
  db.setScript(&script);
  db.commit();

//  QStringList l;
//  l << tr("Script") << _script.getName() << tr("saved");
//  emit signalMessage(l.join(" "));

  accept();
}

