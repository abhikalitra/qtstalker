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

#include "CompositeDialog.h"
#include <qfiledialog.h>

CompositeDialog::CompositeDialog (Config *c) : EditDialog (c)
{
  grid->expand(5, 1);

  insertButton = new QPushButton (tr("Insert"), this);
  connect (insertButton, SIGNAL (clicked()), this, SLOT (newComposite()));
  grid->addWidget(insertButton, 2, 0);

  deleteButton = new QPushButton (tr("Delete"), this);
  connect (deleteButton, SIGNAL (clicked()), this, SLOT (deleteComposite()));
  grid->addWidget(deleteButton, 3, 0);
}

CompositeDialog::~CompositeDialog ()
{
}

void CompositeDialog::newComposite ()
{
  QString selection = QFileDialog::getOpenFileName(config->getData(Config::DataPath), "*", this, "file dialog");
  if (! selection.isNull())
  {
    QStringList l = QStringList::split("/", selection, FALSE);
    QString symbol = l[l.count() - 1];
    table->insertRows(table->numRows(), 1);
    table->setItem(table->numRows() - 1, 0, new QTableItem(table, QTableItem::Never, QString::number(table->numRows())));
    QString s = symbol;
    s.append(" 1");
    table->setItem(table->numRows() - 1, 1, new QTableItem(table, QTableItem::Never, s));
    settings->set(QString::number(table->numRows()), s, Setting::Composite);
  }
}

void CompositeDialog::deleteComposite ()
{
  int row = table->currentRow();
  if (row != -1)
  {
    table->removeRow(row);

    settings->clear();

    int loop;
    for (loop = 0; loop < table->numRows(); loop++)
    {
      table->setText(loop, 0, QString::number(loop + 1));
      settings->set(QString::number(loop + 1), table->text(loop, 1), Setting::Composite);
    }
  }
}

void CompositeDialog::setInsertButton (bool d)
{
  insertButton->setEnabled(d);
}

void CompositeDialog::setDeleteButton (bool d)
{
  deleteButton->setEnabled(d);
}


