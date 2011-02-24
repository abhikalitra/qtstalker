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
#include "../pics/recent_chart.xpm"

#include <QDebug>
#include <QStringList>
#include <QSettings>

RecentCharts::RecentCharts ()
{
  _maxCount = 10;
  createMenu();

  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Recent Charts"));
  setStatusTip(tr("Recent Charts"));
  setIcon(QIcon(recent_chart_xpm));
}

void RecentCharts::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Recent Charts"));
  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(itemSelected(QAction *)));
  setMenu(_menu);

  QSettings settings(g_localSettings);
  QStringList l = settings.value("recent_charts_list").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    bd.setKey(l.at(loop));
    _symbols.insert(l.at(loop), bd);

    QAction *a = _menu->addAction(l.at(loop));
    a->setCheckable(FALSE);
    _actions.append(a);
  }
}

void RecentCharts::addRecentChart (BarData bd)
{
  QString key = bd.key();
  
  if (_symbols.contains(key))
    return;

  if (_symbols.count() == _maxCount)
  {
    QAction *a = _actions.takeLast();
    _menu->removeAction(a);
    _symbols.remove(a->text());
    delete a;
  }

  _symbols.insert(key, bd);

  QAction *a = new QAction(key, _menu);
  a->setCheckable(FALSE);
  _actions.prepend(a);

  QAction *b = _actions.first();
  _menu->insertAction(b, a);
}

void RecentCharts::itemSelected (QAction *d)
{
  emit signalChartSelected(_symbols.value(d->text()));
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

  QSettings settings(g_localSettings);
  settings.setValue("recent_charts_list", l);
  settings.sync();
}
