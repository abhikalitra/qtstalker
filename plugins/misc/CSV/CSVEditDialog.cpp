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

#include "CSVEditDialog.h"
#include "CSVDataBase.h"
#include "Globals.h"

#include <QtDebug>

CSVEditDialog::CSVEditDialog ()
{
  _helpFile = "CSV.html";
  setWindowTitle("QtStalker" + g_session + ": CSV " + tr("Edit Rule"));

  _list->setSelectionMode(QAbstractItemView::SingleSelection);

  CSVDataBase db;
  QStringList l;
  db.getRules(l);
  _list->addItems(l);
}

void CSVEditDialog::done ()
{
  QList<QListWidgetItem *> sl = _list->selectedItems();

  QStringList l;
  l << sl.at(0)->text();
  emit signalSelect(l);
  
  accept();
}

