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
#include "Globals.h"
#include "Strip.h"
#include "ScriptEditDialog.h"

#include <QtDebug>
#include <QLineEdit>

ScriptNewDialog::ScriptNewDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Script"));

  ScriptDataBase db;
  db.getScripts(_scripts);
}

void ScriptNewDialog::done ()
{
  QString name = _name->lineEdit()->text();

  Strip strip;
  strip.verifyText(name);

  if (name.isEmpty())
  {
    setMessage(tr("Name missing."));
    _name->setFocus();
    return;
  }

  // check is name already exists
  if (_scripts.contains(name))
  {
    setMessage(tr("A script with this name already exists."));
    _name->lineEdit()->selectAll();
    _name->setFocus();
    return;
  }

  ScriptEditDialog *dialog = new ScriptEditDialog(name);
  dialog->show();

  accept();
}

