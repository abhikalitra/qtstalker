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


#include "SellDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

SellDraw::SellDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);

  QSettings set(g_globalSettings);
  _settings->setData("PLUGIN", QString("SELL"));
  _settings->setData("COLOR", set.value("default_sell_color", "red").toString());
  _settings->setData("TYPE", QString("Sell"));
  _settings->setData("PRICE", 0);
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("Z", 0);
}

SellDraw::~SellDraw ()
{
  delete _settings;
  detach();
}

void SellDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("DATE")));

  int y = yMap.transform(_settings->getDouble("PRICE"));

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
                  x + 2, y - 5,
                  x + 2, y - 11,
                  x - 2, y - 11,
                  x - 2, y - 5,
                  x - 5, y - 5);

  p->setBrush(_settings->color("COLOR"));

  p->drawPolygon(arrow, Qt::OddEvenFill);

  _selectionArea.clear();

  _selectionArea.append(QRegion(arrow));


  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - (_handleWidth / 2),
                                y - _handleWidth,
                                _handleWidth,
                                _handleWidth,
                                QRegion::Rectangle));

    p->fillRect(x - (_handleWidth / 2),
                y + (_handleWidth / 2),
                _handleWidth,
                _handleWidth,
                _settings->color("COLOR"));
  }
}

int SellDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int SellDraw::isSelected (QPoint p)
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

int SellDraw::isGrabSelected (QPoint p)
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

void SellDraw::setSelected (int d)
{
  _selected = d;
}

Setting * SellDraw::settings ()
{
  return _settings;
}
