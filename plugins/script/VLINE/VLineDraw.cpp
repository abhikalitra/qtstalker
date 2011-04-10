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


#include "VLineDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

VLineDraw::VLineDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);

  QSettings set(g_globalSettings);
  _settings->setData("PLUGIN", QString("VLINE"));
  _settings->setData("COLOR", set.value("default_vline_color", "red").toString());
  _settings->setData("TYPE", QString("VLine"));
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("Z", 0);
}

VLineDraw::~VLineDraw ()
{
  delete _settings;
  detach();
}

void VLineDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("DATE")));

  p->setPen(_settings->color("COLOR"));

  p->drawLine (x, 0, x, p->window().height());

  _selectionArea.clear();

  QPolygon array;
  array.putPoints(0,
  	          4,
		  x - 2, 0,
		  x + 2, 0,
		  x + 2, p->window().height(),
		  x - 2, p->window().height());

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    int t = (int) p->window().height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(x - (_handleWidth / 2),
		                  t * loop,
		                  _handleWidth,
		                  _handleWidth,
		                  QRegion::Rectangle));

      p->fillRect(x - (_handleWidth / 2),
		  t * loop,
		  _handleWidth,
		  _handleWidth,
		  _settings->color("COLOR"));
    }
  }
}

int VLineDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int VLineDraw::isSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion r = _selectionArea.at(loop);
    if (r.contains(p))
      return 1;
  }

  return 0;
}

int VLineDraw::isGrabSelected (QPoint p)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion r = _grabHandles.at(loop);
    if (r.contains(p))
      return loop + 1;
  }

  return 0;
}

void VLineDraw::setSelected (int d)
{
  _selected = d;
}

Setting * VLineDraw::settings ()
{
  return _settings;
}