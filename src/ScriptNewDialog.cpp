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
#include "ScriptDataBase.h"

#include <QtDebug>
#include <QLabel>

ScriptNewDialog::ScriptNewDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle(tr("QtStalker: New Script"));

  createMainPage();
}

void ScriptNewDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // name
  QLabel *label = new QLabel(tr("Script Name"));
  grid->addWidget(label, row, col++);

  _name = new QLineEdit;
  grid->addWidget(_name, row++, col--);
  
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, QString());
}

void ScriptNewDialog::done ()
{
  QString name = _name->text();
  if (name.isEmpty())
  {
    reject();
    return;
  }

  // remove any forbidden sql characters
  name = name.remove(QString("'"), Qt::CaseSensitive);

  // check is name already exists
  QStringList l;
  ScriptDataBase db;
  db.getScripts(l);
  if (l.contains(name))
  {
    Dialog *dialog = new Dialog(Dialog::_Message, 0);
    dialog->setWindowTitle(tr("Qtstalker: Error New Script"));
    dialog->setMessage(tr("A script with this name already exists."));
    dialog->show();
    return;
  }

  emit signalNewScript(name);
  
  accept();
}

