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

#include "RecentCharts.h"
#include "Config.h"

#include <QDebug>
#include <QString>
#include <QStringList>

RecentCharts::RecentCharts (QToolBar *tb)
{
  setMaxCount(10);
  setSizeAdjustPolicy(QComboBox::AdjustToContents);
  setCurrentIndex(0);
  setToolTip(tr("Recent Charts"));
  connect(this, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));
  tb->addWidget(this);
}

void RecentCharts::addRecentChart (BarData bd)
{
  if (_group.contains(bd))
    return;

  if (count() == maxCount())
    _group.deleteItem(maxCount() - 1);

  _group.prepend(bd);

  insertItem(0, bd.getSymbol());
}

void RecentCharts::itemSelected (int row)
{
  QString s = itemText(row);
  removeItem(row);
  insertItem(0, s);

  _group.move(row, 0);

  setCurrentIndex(0);

  BarData bd;
  if (_group.getItem(0, bd))
    return;

  emit signalChartSelected(bd);
}

void RecentCharts::save ()
{
  QStringList l;
  _group.getStringList(l);

  Config config;
  config.setData(Config::RecentChartsList, l);
}

void RecentCharts::load ()
{
  QStringList l;
  Config config;
  config.getData(Config::RecentChartsList, l);

  int loop;
  for (loop = 0; loop < l.count(); loop = loop + 2)
  {
    BarData bd;
    bd.setExchange(l[loop]);
    bd.setSymbol(l[loop + 1]);
    _group.append(bd);
    addItem(l[loop + 1]);
  }
}

