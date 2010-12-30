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

#include "MiddleMan.h"

#include <QDebug>

MiddleMan::MiddleMan (QObject *parent) : QObject(parent)
{
}

void MiddleMan::chartPanelRefresh ()
{
  emit signalChartPanelRefresh();
}

void MiddleMan::chartPanelSearch (QString exchange, QString symbol)
{
  emit signalChartPanelSearch(exchange, symbol);
}

void MiddleMan::groupPanelRefresh ()
{
  emit signalGroupPanelRefresh();
}

void MiddleMan::plotBackgroundColor (QColor d)
{
  emit signalPlotBackgroundColor(d);
}

void MiddleMan::plotFont (QFont d)
{
  emit signalPlotFont(d);
}

void MiddleMan::gridColor (QColor d)
{
  emit signalGridColor(d);
}

void MiddleMan::grid (int d)
{
  emit signalGrid((bool) d);
}

void MiddleMan::crosshairsColor (QColor d)
{
  emit signalCrosshairsColor(d);
}

void MiddleMan::crosshairs (int d)
{
  emit signalCrosshairs((bool) d);
}

void MiddleMan::indicatorNew (QString d)
{
  emit signalIndicatorNew(d);
}

void MiddleMan::indicatorDelete (QStringList d)
{
  emit signalIndicatorDelete(d);
}

void MiddleMan::scriptRun (QString d)
{
  emit signalScriptRun(d);
}

void MiddleMan::plotTabPosition (int d)
{
  emit signalPlotTabPosition(d);
}

void MiddleMan::plotUpdate (QString d)
{
  emit signalPlotUpdate(d);
}

void MiddleMan::chartObjectDelete (QStringList d)
{
  emit signalChartObjectDelete(d);
}

void MiddleMan::chartObjectUpdate (QString d)
{
  emit signalChartObjectUpdate(d);
}
