/*
 *  TestRankings
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#include <QVBoxLayout>
#include <QtDebug>
#include <QSqlQuery>
#include <QTreeWidgetItem>
#include <QStringList>

#include "TestRankings.h"
#include "TestDataBase.h"

TestRankings::TestRankings ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(0);
  setLayout(vbox);

  rankTree = new QTreeWidget;
  rankTree->setColumnCount(4);
  rankTree->setRootIsDecorated(FALSE);
  QStringList l;
  l << tr("Test Name") << tr("Win/Loss Ratio") << tr("Gross Profit") << tr("Net Profit");
  rankTree->setHeaderLabels(l);
  vbox->addWidget(rankTree);

  update();
}

void TestRankings::update ()
{
  TestDataBase db;
  QSqlQuery q;
  db.getRankings(q);

  rankTree->clear();

  rankTree->setSortingEnabled(FALSE);

  while (q.next())
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(rankTree);
    item->setText(0, q.value(0).toString());
    item->setText(1, q.value(1).toString());
    item->setText(2, q.value(2).toString());
    item->setText(3, q.value(3).toString());
  }

  int loop;
  for (loop = 0; loop < rankTree->columnCount(); loop++)
    rankTree->resizeColumnToContents(loop);

  rankTree->setSortingEnabled(TRUE);
}

