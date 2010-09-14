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

#include "ScriptSearchDialog.h"
#include "ScriptDataBase.h"

#include <QtDebug>
#include <QLabel>

ScriptSearchDialog::ScriptSearchDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle(tr("QtStalker: Search Script"));

  createMainPage();
}

void ScriptSearchDialog::createMainPage ()
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

  _search = new QLineEdit;
  _search->setToolTip(tr("Enter either a specific name like MyScript or\na partial match like %Script or %Scr%"));
  grid->addWidget(_search, row++, col--);
  
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, QString());
}

void ScriptSearchDialog::done ()
{
  QString ss = _search->text();
  if (ss.isEmpty())
  {
    reject();
    return;
  }

  ScriptDataBase db;
  QStringList l;
  db.getScriptSearch(ss, l);

  emit signalSearch(ss, l);
  
  accept();
}

