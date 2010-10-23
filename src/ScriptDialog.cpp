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

#include "ScriptDialog.h"
#include "Config.h"
#include "Globals.h"
#include "Strip.h"

#include <QtDebug>
#include <QFormLayout>
#include <QDir>

ScriptDialog::ScriptDialog (QString name)
{
  _editFlag = 0;
  
  QString cap = "Qtstalker" + g_session + ": ";
  
  if (! name.isEmpty())
  {
    _script.setName(name);
    _db.getScript(&_script);

    cap.append(tr("Edit Script") + " " + name);
    setWindowTitle(cap);

    _editFlag = 1;
  }
  else
  {
    cap.append(tr("Run Script"));
    setWindowTitle(cap);
  }

  createMainPage();

  _saveFlag = 0;
  buttonStatus();
}

void ScriptDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // command
  QString s = _script.getCommand();
  if (s.isEmpty())
    s = "perl";

  _command = new QLineEdit(s);
  _command->setToolTip(tr("Interpreter command and switches eg. perl -l -T"));
  connect(_command, SIGNAL(textEdited(const QString &)), this, SLOT(buttonStatus()));
  form->addRow(tr("Command"), _command);
  
  // file
  s = _script.getFile();
  if (s.isEmpty())
  {
    Config config;
    config.getData(Config::LastScriptPath, s);
    if (s.isEmpty())
      s = QDir::homePath();
  }

  _file = new FileButton(this, s);
  _file->setToolTip(tr("The script location"));
  connect(_file, SIGNAL(signalFileChanged()), this, SLOT(buttonStatus()));
  form->addRow(tr("Script File"), _file);

  // check if run dialog format
  s = _script.getName();
  if (! s.isEmpty())
  {
    // comment
    s = _script.getComment();
    
    _comment = new QTextEdit;
    _comment->append(s);
    connect(_comment, SIGNAL(textChanged()), this, SLOT(buttonStatus()));
    form->addRow(tr("Comment"), _comment);
  }

  _tabs->addTab(w, tr("Settings"));
}

void ScriptDialog::buttonStatus ()
{
  if (_editFlag)
    _saveFlag = 1;
  
  bool status = TRUE;
  if (_command->text().isEmpty())
    status = FALSE;
  if (_file->getFile().isEmpty())
    status = FALSE;
  _okButton->setEnabled(status);
}

void ScriptDialog::confirmYes ()
{
  done();
}

void ScriptDialog::cancel ()
{
  if (_saveFlag == 1 && _editFlag ==  1)
  {
    setMessage(tr("Settings modified. Save changes?"));
    setConfirm();
    _okButton->setEnabled(FALSE);
    _cancelButton->setEnabled(FALSE);
  }
  else
    reject();
}

void ScriptDialog::done ()
{
  if (_script.getName().isEmpty())
    emit signalRunScript(_command->text(), _file->getFile());
  else
  {
    Strip strip;
    
    QString s = _command->text();
    strip.verifyText(s);
    _script.setCommand(s);

    s = _file->getFile();
    _script.setFile(s);
    
    Config config;
    config.transaction();
    config.setData(Config::LastScriptPath, s);
    config.commit();

    s = _comment->toPlainText();
    strip.verifyText(s);
    _script.setComment(s);
  
    _db.transaction();
    _db.setScript(&_script);
    _db.commit();

    QStringList l;
    l << tr("Script") << _script.getName() << tr("saved");
    emit signalMessage(l.join(" "));
  }

  accept();
}

