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

#include "ScriptDeleteDialog.h"
#include "Globals.h"

#include <QtDebug>

ScriptDeleteDialog::ScriptDeleteDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Scripts"));

  QStringList l;
  _db.getScripts(l);
  setList(l);

  _tabs->setTabText(0, tr("Script Name"));
}

void ScriptDeleteDialog::done ()
{
  setMessage(tr("Delete selected scripts. Not the actual scripts, just the database records."));
  setConfirm();
}

void ScriptDeleteDialog::confirmYes ()
{
  deleteScripts();
}

void ScriptDeleteDialog::confirmNo ()
{
  unsetConfirm();
}

void ScriptDeleteDialog::deleteScripts ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  _db.transaction();

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    QString s = item->text();
    
    Script script;
    script.setName(s);
   _db.deleteScript(&script);
  }

  _db.commit();

  emit signalMessage(QString(tr("Scripts deleted.")));

  emit signalDelete();
  
  accept();
}

