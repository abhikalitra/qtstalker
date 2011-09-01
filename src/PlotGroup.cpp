/*
 *  Qtstalker stock charter
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

#include "PlotGroup.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

PlotGroup::PlotGroup (QWidget *w) : QSplitter (Qt::Vertical, w)
{
  setOpaqueResize(FALSE);

  int loop = 0;
  for (; loop < 3; loop++)
  {
    QTabWidget *tw = new QTabWidget;
    tw->hide();
    addWidget(tw);
    _tabs << tw;
  }
}

int PlotGroup::setPlot (Plot *p)
{
  if (p->name().isEmpty())
    return 1;

  if (_plots.contains(p->name()))
    return 1;

  if (p->row() < 0 || p->row() >= _tabs.count())
    return 1;

  QTabWidget *tw = _tabs.at(p->row());

  tw->insertTab(p->col(), p, p->name());

  _plots.insert(p->name(), p);

  if (tw->isHidden())
    tw->show();

  return 0;
}

Plot * PlotGroup::plot (QString d)
{
  return _plots.value(d);
}

QList<QString> PlotGroup::plots ()
{
  return _plots.keys();
}

void PlotGroup::removePlot (QString d)
{
  Plot *p = _plots.value(d);
  if (! p)
    return;

  QTabWidget *tw = _tabs.at(p->row());
  if (! tw)
    return;

  tw->removeTab(p->col());

  if (tw->count() == 0)
    tw->hide();

  _plots.remove(d);
  delete p;
}

void PlotGroup::loadSettings ()
{
  QSettings settings(g_localSettings);
  restoreState(settings.value("plot_group_state").toByteArray());
}

void PlotGroup::saveSettings ()
{
  QSettings settings(g_localSettings);
  settings.setValue("plot_group_state", saveState());
  settings.sync();
}
