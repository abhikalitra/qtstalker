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


#include "TLineDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

TLineDraw::TLineDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");

  QSettings set(g_globalSettings);
  _settings->setData("PLUGIN", QString("TLINE"));
  _settings->setData("COLOR", set.value("default_tline_color", "red").toString());
  _settings->setData("TYPE", QString("TLine"));
  _settings->setData("PRICE", 0);
  _settings->setData("PRICE2", 0);
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("DATE2", QDateTime::currentDateTime());
  _settings->setData("EXTEND", set.value("default_tline_extend", 0).toInt());
  _settings->setData("Z", 99);
}

TLineDraw::~TLineDraw ()
{
  delete _settings;
  detach();
}

void TLineDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("DATE")));
  if (x == -1)
    return;

  int x2 = xMap.transform(dsd->x(_settings->dateTime("DATE2")));
  if (x2 == -1)
    return;

  int y = yMap.transform(_settings->getDouble("PRICE"));
  int y2 = yMap.transform(_settings->getDouble("PRICE2"));

  p->setPen(_settings->color("COLOR"));

  p->drawLine (x, y, x2, y2);

  // save old values;
  int tx2 = x2;
  int ty2 = y2;
  int tx = x;
  int ty = y;

  if (_settings->getInt("EXTEND"))
  {
    int ydiff = y - y2;
    int xdiff = x2 - x;
    if (xdiff > 0)
    {
      while (x2 < p->window().width())
      {
        x = x2;
        y = y2;
        x2 = x2 + xdiff;
        y2 = y2 - ydiff;
        p->drawLine (x, y, x2, y2);
      }
    }
  }

  // store the selectable area the line occupies
  _selectionArea.clear();

  QPolygon array;
//  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y2 + 4, x2, y2 - 4);
  array.putPoints(0, 4, tx, ty - 4, tx, ty + 4, x2, y2 + 4, x2, y2 - 4);

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(tx,
		                ty - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx,
		ty - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("COLOR"));

    _grabHandles.append(QRegion(tx2,
		                ty2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(tx2,
		ty2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("COLOR"));
  }
}

int TLineDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int TLineDraw::isSelected (QPoint p)
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

int TLineDraw::isGrabSelected (QPoint p)
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

void TLineDraw::setSelected (int d)
{
  _selected = d;
}

Setting * TLineDraw::settings ()
{
  return _settings;
}