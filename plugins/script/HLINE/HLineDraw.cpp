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


#include "HLineDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

HLineDraw::HLineDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);

  QSettings set(g_globalSettings);
  _settings->setData("PLUGIN", QString("HLINE"));
  _settings->setData("COLOR", set.value("default_hline_color", "white").toString());
  _settings->setData("TYPE", QString("HLine"));
  _settings->setData("PRICE", 0);
  _settings->setData("Z", 0);
}

HLineDraw::~HLineDraw ()
{
  delete _settings;
  detach();
}

void HLineDraw::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
  p->setPen(_settings->color("COLOR"));

  int y = yMap.transform(_settings->getDouble("PRICE"));

  // test start
  QString s = " " + _settings->data("PRICE"); // prepend space so we can clearly read text
  QFontMetrics fm = p->fontMetrics();
  QRect rc = p->boundingRect(0, y - (fm.height() / 2), 1, 1, 0, s);
  p->fillRect(rc, plot()->canvasBackground()); // fill in behind text first
  p->drawText(rc, s); // draw text
  p->drawRect(rc); // draw a nice little box around text

  p->drawLine (rc.width(), y, p->window().width(), y);

  _selectionArea.clear();

  QPolygon array;
  array.putPoints(0,
                  4,
                  0,
                  y - 4,
                  0,
                  y + 4,
                  p->window().width(),
                  y + 4,
                  p->window().width(),
                  y - 4);

  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    int t = (int) p->window().width() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      _grabHandles.append(QRegion(t * loop,
                                  y - (_handleWidth / 2),
                                  _handleWidth,
                                  _handleWidth,
                                  QRegion::Rectangle));

      p->fillRect(t * loop,
                  y - (_handleWidth / 2),
                  _handleWidth,
                  _handleWidth,
                  _settings->color("COLOR"));
    }
  }
}

int HLineDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int HLineDraw::isSelected (QPoint p)
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

int HLineDraw::isGrabSelected (QPoint p)
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

void HLineDraw::setSelected (int d)
{
  _selected = d;
}

Setting * HLineDraw::settings ()
{
  return _settings;
}
