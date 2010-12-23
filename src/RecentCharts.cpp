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
#include "Globals.h"

#include <QDebug>
#include <QString>
#include <QStringList>
#include <QSettings>

RecentCharts::RecentCharts (QToolBar *tb)
{
  setMaxCount(10);
  setSizeAdjustPolicy(QComboBox::AdjustToContents);
  setCurrentIndex(0);
  setToolTip(tr("Recent Charts"));
  connect(this, SIGNAL(activated(int)), this, SLOT(itemSelected(int)));
  tb->addWidget(this);

  load();
}

RecentCharts::~RecentCharts ()
{
//  save();
}

void RecentCharts::addRecentChart (BarData bd)
{
  QString key = bd.key();
  
  if (_symbols.contains(key))
    return;

  if (count() == maxCount())
    _symbols.remove(itemText(maxCount() - 1));

  _symbols.insert(key, bd);

  insertItem(0, key);
}

void RecentCharts::itemSelected (int row)
{
  QString s = itemText(row);
  removeItem(row);
  insertItem(0, s);

  setCurrentIndex(0);

  emit signalChartSelected(_symbols.value(s));
}

void RecentCharts::save ()
{
  QStringList l;
  QHashIterator<QString, BarData> it(_symbols);
  while (it.hasNext())
  {
    it.next();
    BarData bd = it.value();
    l << bd.key();
  }

  QSettings settings(g_settingsFile);
  settings.setValue("recent_charts_list", l);
  settings.sync();
}

void RecentCharts::load ()
{
  QSettings settings(g_settingsFile);
  QStringList l = settings.value("recent_charts_list").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    bd.setKey(l.at(loop));
    _symbols.insert(l.at(loop), bd);
    addItem(l.at(loop));
  }
}
