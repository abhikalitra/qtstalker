/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "DataWindow.h"
#include <qlayout.h>
#include <qpushbutton.h>

DataWindow::DataWindow (int cols, int rows) : QDialog (0, "DataWindow", FALSE, WDestructiveClose)
{
  resize(500, 350);

  QVBoxLayout *vbox = new QVBoxLayout (this);
  vbox->setSpacing(5);
  vbox->setMargin(5);

  table = new QTable(rows, cols, this);
  table->setSelectionMode(QTable::Single);
  hHeader = table->horizontalHeader();
  vbox->addWidget (table);

  QPushButton *button = new QPushButton (tr("OK"), this);
  connect (button, SIGNAL (clicked()), this, SLOT (reject()));
  vbox->addWidget (button);
}

DataWindow::~DataWindow ()
{
}

void DataWindow::setData (int row, int col, QString data)
{
  table->setText(row, col, data);
}

void DataWindow::setHeader (int col, QString d)
{
  if (col >= table->numCols())
    table->setNumCols(table->numCols() + 1);
  table->setColumnWidth(col, 70);
  hHeader->setLabel(col, d);
}

