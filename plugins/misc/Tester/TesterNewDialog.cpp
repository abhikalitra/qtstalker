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

#include "TesterNewDialog.h"
#include "Globals.h"
#include "TesterDataBase.h"
#include "TesterSettingsDialog.h"

#include <QtDebug>
#include <QLineEdit>

TesterNewDialog::TesterNewDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": Tester " + tr("New Back Tester"));

  TesterDataBase db;
  db.getRules(_rules);
  setList(_rules);
}

void TesterNewDialog::done ()
{
  QString name = _name->lineEdit()->text();

  // remove any forbidden sql characters
  name = name.remove(QString("'"), Qt::CaseSensitive);

  if (_rules.contains(name))
  {
    setMessage(tr("Duplicate rule name. Enter a unique name."));
    _name->lineEdit()->selectAll();
    _name->setFocus();
    return;
  }

  TesterSettingsDialog *dialog = new TesterSettingsDialog(name);
  dialog->show();
  
  accept();
}
