/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "CUSDialog.h"
#include "../../../src/newchart.xpm"
#include "../../../src/delete.xpm"
#include <qinputdialog.h>
#include <qmessagebox.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qframe.h>

CUSDialog::CUSDialog () : QTabDialog (0, "CUSDialog", TRUE)
{
  setCaption(tr("CUS Indicator"));

  QWidget *w = new QWidget(this);
  
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(5);
  vbox->setSpacing(0);
  
  toolbar = new Toolbar(w, 30, 30);
  vbox->addWidget(toolbar);
  
  toolbar->addButton("add", newchart, tr("Add"));
  QObject::connect(toolbar->getButton("add"), SIGNAL(clicked()), this, SLOT(addItem()));
  
  toolbar->addButton("delete", deleteitem, tr("Delete"));
  QObject::connect(toolbar->getButton("delete"), SIGNAL(clicked()), this, SLOT(deleteItem()));
  
  vbox->addSpacing(10);
  
  list = new QTable(0, 2, w);
  list->setSelectionMode(QTable::Single);
  list->setSorting(FALSE);
  list->horizontalHeader()->setLabel(0, tr("Function"));
  list->horizontalHeader()->setLabel(1, tr("Plot"));
  list->setColumnWidth(0, 275);
  list->setColumnWidth(1, 50);
  vbox->addWidget(list);
  
  addTab(w, tr("Settings"));

  setOkButton();
  setCancelButton();

  resize(400, 300);
}

CUSDialog::~CUSDialog ()
{
}

void CUSDialog::addItem ()
{
  list->setNumRows(list->numRows() + 1);
  
  list->setText(list->numRows() - 1, 0, QString::null);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  check->setChecked(FALSE);
  list->setItem(list->numRows() - 1, 1, check);
}

void CUSDialog::deleteItem ()
{
  list->removeRow(list->currentRow());
}

void CUSDialog::setLine (QString d, bool c)
{
  list->setNumRows(list->numRows() + 1);
    
  list->setText(list->numRows() - 1, 0, d);
  
  QCheckTableItem *check = new QCheckTableItem(list, QString::null);
  if (! c)
    check->setChecked(FALSE);
  else
    check->setChecked(TRUE);
  list->setItem(list->numRows() - 1, 1, check);
}

int CUSDialog::getLines ()
{
  return list->numRows();
}

QString CUSDialog::getFunction (int row)
{
  return list->text(row, 0);
}

bool CUSDialog::getPlot (int row)
{
  QCheckTableItem *check = (QCheckTableItem *) list->item(row, 1);
  return check->isChecked();
}

