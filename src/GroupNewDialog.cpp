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

#include "GroupNewDialog.h"
#include "GroupDataBase.h"
#include "Globals.h"
#include "Strip.h"

#include <QtDebug>
#include <QLineEdit>

GroupNewDialog::GroupNewDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("New Group"));

  GroupDataBase db;
  db.getAllGroupsList(_groups);
  setList(_groups);
}

void GroupNewDialog::done ()
{
  QString name = _name->lineEdit()->text();

  Strip strip;
  strip.verifyText(name);

  if (_groups.contains(name))
  {
    setMessage(tr("A group with this name already exists."));
    return;
  }

  GroupDataBase db;
  Group g;
  g.setName(name);
  db.transaction();
  db.setGroup(g);
  db.commit();

  QStringList ml;
  ml << tr("Group") << name << tr("created");
  emit signalMessage(ml.join(" "));

  emit signalNew();
  
  accept();
}

