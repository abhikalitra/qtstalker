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


#include "RetracementDraw.h"
#include "DateScaleDraw.h"
#include "Globals.h"

#include <QDebug>
#include <qwt_plot.h>
#include <QPolygon>
#include <QSettings>

RetracementDraw::RetracementDraw ()
{
  _settings = new Setting;
  _selected = 0;
  _handleWidth = 6;
  setYAxis(QwtPlot::yRight);

  QSettings set(g_globalSettings);
  _settings->setData("PLUGIN", QString("RETRACEMENT"));
  _settings->setData("COLOR", set.value("default_retracement_color", "red").toString());
  _settings->setData("TYPE", QString("Retracement"));
  _settings->setData("DATE", QDateTime::currentDateTime());
  _settings->setData("DATE2", QDateTime::currentDateTime());
  _settings->setData("Z", 0);
  _settings->setData("HIGH", 0);
  _settings->setData("LOW", 0);
  _settings->setData("EXTEND", 0);
  _settings->setData("LINE1", set.value("default_retracement_line1", 0.382).toDouble());
  _settings->setData("LINE2", set.value("default_retracement_line2", 0.5).toDouble());
  _settings->setData("LINE3", set.value("default_retracement_line3", 0.618).toDouble());
  _settings->setData("LINE4", set.value("default_retracement_line4", 0).toDouble());
  _settings->setData("LINE5", set.value("default_retracement_line5", 0).toDouble());
  _settings->setData("LINE6", set.value("default_retracement_line6", 0).toDouble());
}

RetracementDraw::~RetracementDraw ()
{
  delete _settings;
  detach();
}

void RetracementDraw::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->dateTime("DATE")));
  if (x == -1)
    return;

  QDateTime dt = _settings->dateTime("DATE2");
  if (_settings->getInt("EXTEND"))
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));
  if (x2 == -1)
    return;

  p->setPen(_settings->color("COLOR"));

  QPolygon array;

  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings->getDouble("LINE1"));
  lineList.append(_settings->getDouble("LINE2"));
  lineList.append(_settings->getDouble("LINE3"));
  lineList.append(_settings->getDouble("LINE4"));
  lineList.append(_settings->getDouble("LINE5"));
  lineList.append(_settings->getDouble("LINE6"));

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings->getDouble("HIGH") - _settings->getDouble("LOW");
      double r = 0;
      if (td < 0)
        r = _settings->getDouble("LOW") + (range * td);
      else
      {
        if (td > 0)
          r = _settings->getDouble("LOW") + (range * td);
        else
        {
          if (td < 0)
            r = _settings->getDouble("HIGH");
          else
            r = _settings->getDouble("LOW");
        }
      }

      int y = yMap.transform(r);
      p->drawLine (x, y, x2, y);
      p->drawText(x, y - 1, QString::number(td * 100) + "% - " + QString::number(r));

      array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
      _selectionArea.append(QRegion(array));
    }
  }

  // draw the low line
  int y = yMap.transform(_settings->getDouble("LOW"));
  p->drawLine (x, y, x2, y);
  p->drawText(x, y - 1, "0% - " + _settings->data("LOW"));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings->getDouble("HIGH"));
  p->drawLine (x, y2, x2, y2);
  p->drawText(x, y2 - 1, "100% - " + _settings->data("HIGH"));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  _selectionArea.append(QRegion(array));

  if (_selected)
  {
    _grabHandles.clear();

    //top left corner
    _grabHandles.append(QRegion(x, y2 - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x,
		y2 - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("COLOR"));

    //bottom right corner
    _grabHandles.append(QRegion(x2, y - (_handleWidth / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x2,
		y - (_handleWidth / 2),
		_handleWidth,
		_handleWidth,
		_settings->color("COLOR"));
  }
}

int RetracementDraw::rtti () const
{
  return Rtti_PlotUserItem;
}

int RetracementDraw::isSelected (QPoint p)
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

int RetracementDraw::isGrabSelected (QPoint p)
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

void RetracementDraw::setSelected (int d)
{
  _selected = d;
}

Setting * RetracementDraw::settings ()
{
  return _settings;
}
