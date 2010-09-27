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

#include "IndicatorDeleteDialog.h"
#include "Globals.h"

#include <QtDebug>

IndicatorDeleteDialog::IndicatorDeleteDialog ()
{
  setWindowTitle("QtStalker" + g_session + ": " + tr("Delete Indicator"));

  QStringList l;
  _db.getActiveIndicatorList(l);
  setList(l);  

  _tabs->setTabText(0, tr("Indicator Name"));
}

void IndicatorDeleteDialog::done ()
{
  switch (_confirmFlag)
  {
    case _ConfirmNone:
      setMessage(tr("Confirm Delete:"));
      setConfirm(tr("Delete selected indicators."));
      break;
    case _ConfirmNo:
      unsetConfirm();
      return;
    case _ConfirmYes:
      deleteIndicators();
      return;
      break;
    default:
      break;
  }
}

void IndicatorDeleteDialog::deleteIndicators ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  _db.transaction();

  QStringList l;
  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);

    QString s = item->text();
    l.append(s);
    
    _db.deleteIndicator(s);
  }

  _db.commit();

  emit signalSelect(l);

  emit signalMessage(QString(tr("Indicators deleted.")));

  accept();
}

