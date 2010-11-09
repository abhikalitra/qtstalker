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

#include "GroupDeleteItemsDialog.h"
#include "Globals.h"

#include <QtDebug>

GroupDeleteItemsDialog::GroupDeleteItemsDialog (QString name)
{
  _group.setName(name);
  _db.getGroup(_group);
  _helpFile = "main.html";

  QStringList l;
  _group.getStringList(l);
  setList(l);
  
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Group Items"));

  _tabs->setTabText(0, tr("Group Contents"));
}

void GroupDeleteItemsDialog::confirmYes ()
{
  deleteItems();
}

void GroupDeleteItemsDialog::confirmNo ()
{
  unsetConfirm();
}

void GroupDeleteItemsDialog::done ()
{
  setMessage(tr("Delete selected group items?"));
  setConfirm();
}

void GroupDeleteItemsDialog::deleteItems ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  _db.transaction();
  
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    QString s = item->text();
    _group.deleteSymbol(s);
  }

  _db.setGroup(_group);
  _db.commit();

  emit signalMessage(QString(tr("Group Item(s) deleted.")));

  emit signalDelete();
  
  accept();
}
