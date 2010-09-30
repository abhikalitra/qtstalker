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

#include "ScannerDeleteDialog.h"
#include "Globals.h"
#include "ScannerDataBase.h"

#include <QtDebug>

ScannerDeleteDialog::ScannerDeleteDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Scanners"));

  QStringList l;
  ScannerDataBase db;
  db.getScanners(l);
  setList(l);
}

void ScannerDeleteDialog::done ()
{
  switch (_confirmFlag)
  {
    case _ConfirmNone:
      setMessage(tr("Confirm Delete:"));
      setConfirm(tr("Delete selected scanners."));
      break;
    case _ConfirmNo:
      unsetConfirm();
      return;
    case _ConfirmYes:
      deleteScanners();
      return;
      break;
    default:
      break;
  }
}

void ScannerDeleteDialog::deleteScanners ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  ScannerDataBase db;
  db.transaction();
  
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);

    QString s = item->text(); 
    db.deleteScanner(s);
  }

  db.commit();

  emit signalMessage(tr("Scanner(s) deleted."));

  emit signalDelete();
  
  accept();
}

