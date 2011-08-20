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


#include "ChartObjectVLine.h"
#include "Globals.h"
#include "SettingDateTime.h"
#include "SettingInteger.h"
#include "SettingColor.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectVLine::ChartObjectVLine ()
{
  _type = "VLine";

  SettingDateTime *dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE");
  _settings->set(dt);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  _settings->set(sc);

  SettingInteger *si = new SettingInteger(0, 0, 1, 99, 0);
  si->setKey("Z");
  _settings->set(si);
}

void ChartObjectVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get("DATE")->getDateTime()));

  p->setPen(_settings->get("COLOR")->getColor());

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
		  _settings->get("COLOR")->getColor());
    }
  }
}

int ChartObjectVLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _type);

  QDateTime dt = _settings->get("DATE")->getDateTime();
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  return 0;
}

void ChartObjectVLine::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = plot()->canvasMap(QwtPlot::xBottom);
      int x = map.invTransform((double) p.x());

      DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
      QDateTime dt;
      dsd->date(x, dt);

      Setting *set = _settings->get("DATE");
      set->setDateTime(dt);

      plot()->replot();

      QString s = _settings->get("DATE")->toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectVLine::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place VLine object..."));
  return 0;
}
