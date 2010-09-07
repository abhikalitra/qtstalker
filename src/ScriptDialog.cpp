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
#include "ScriptDataBase.h"
#include "Config.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QDir>

ScriptDialog::ScriptDialog (QString name) : QDialog (0, 0)
{
  setAttribute(Qt::WA_DeleteOnClose);

  if (! name.isEmpty())
  {
    ScriptDataBase db;
    _script.setName(name);
    db.getScript(&_script);
    
    QString s = tr("Qtstalker: Edit Script ") + name;
    setWindowTitle(s);
  }
  else
    setWindowTitle(tr("Run Script"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // command
  QLabel *label = new QLabel(tr("Command"));
  grid->addWidget(label, row, col++);

  QString s = _script.getCommand();
  if (s.isEmpty())
    s = "perl";

  _command = new QLineEdit(s);
  _command->setToolTip(tr("Interpreter command and switches eg. perl -l -T"));
  grid->addWidget(_command, row++, col--);
  
  // file
  label = new QLabel(tr("Script File"));
  grid->addWidget(label, row, col++);

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
  grid->addWidget(_file, row++, col--);

  // check if run dialog format
  if (! name.isEmpty())
  {
    // comment
    label = new QLabel(tr("Comment"));
    grid->addWidget(label, row, col++);

    s = _script.getComment();

    _comment = new QLineEdit(s);
    grid->addWidget(_comment, row++, col--);
  }

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  vbox->addStretch(1);
}

void ScriptDialog::done ()
{
  if (_script.getName().isEmpty())
    emit signalRunScript(_command->text(), _file->getFile());
  else
  {
    ScriptDataBase db;
    QString s = _command->text();
    _script.setCommand(s);

    s = _file->getFile();
    _script.setFile(s);

    s = _comment->text();
    _script.setComment(s);
  
    db.setScript(&_script);

    emit signalScriptChanged(_script.getName());
  }

  accept();
}

