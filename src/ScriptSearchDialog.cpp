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
#include "Globals.h"

#include <QtDebug>

ScriptSearchDialog::ScriptSearchDialog () : Dialog (Dialog::_Dialog, 0)
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Search Script"));

  createMainPage();
}

void ScriptSearchDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  w->setLayout(vbox);

  _search = new QLineEdit;
  _search->setToolTip(tr("Enter either a specific name like MyScript or\na partial match like %Script or %Scr%"));
  vbox->addWidget(_search);
  
  _tabs->addTab(w, tr("Script Name"));
}

void ScriptSearchDialog::done ()
{
  QString ss = _search->text();
  if (ss.isEmpty())
  {
    reject();
    return;
  }

  emit signalSearch(ss);
  
  accept();
}

