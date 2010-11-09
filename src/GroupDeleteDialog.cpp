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

#include "GroupDeleteDialog.h"
#include "Globals.h"

#include <QtDebug>

GroupDeleteDialog::GroupDeleteDialog ()
{
  _helpFile = "main.html";
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Groups"));

  QStringList l;
  _db.getAllGroupsList(l);
  setList(l);
}

void GroupDeleteDialog::done ()
{
  setMessage(tr("Delete selected groups?"));
  setConfirm();
}

void GroupDeleteDialog::confirmYes ()
{
  deleteGroups();
}

void GroupDeleteDialog::confirmNo ()
{
  unsetConfirm();
}

void GroupDeleteDialog::deleteGroups ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  _db.transaction();
  
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);

    QString s = item->text(); 
    _db.deleteGroup(s);
  }

  _db.commit();

  emit signalMessage(tr("Group(s) deleted."));

  emit signalDelete();
  
  accept();
}

