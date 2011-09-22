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


#include "ChartObjectHLine.h"
#include "Globals.h"
#include "Strip.h"
#include "ChartObjectData.h"
#include "SettingDouble.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingColor.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectHLine::ChartObjectHLine ()
{
  _settings->set(ChartObjectData::_TYPE, new SettingString(QString("HLine")));
  _settings->set(ChartObjectData::_PRICE, new SettingDouble(0));
  _settings->set(ChartObjectData::_COLOR, new SettingColor(QColor(Qt::red)));
  _settings->set(ChartObjectData::_Z, new SettingInteger(1));
  _settings->set(ChartObjectData::_PEN, new SettingInteger(1));
}

void ChartObjectHLine::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
  p->setPen(_settings->get(ChartObjectData::_COLOR)->toColor());

  int y = yMap.transform(_settings->get(ChartObjectData::_PRICE)->toDouble());

  // test start
  Strip strip;
  QString ts;
  strip.strip(_settings->get(ChartObjectData::_PRICE)->toDouble(), 4, ts);
  QString s = " " + ts; // prepend space so we can clearly read text
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
                  _settings->get(ChartObjectData::_COLOR)->toColor());
    }
  }
}

int ChartObjectHLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE)->toString());
  info.insert(QObject::tr("Price"), _settings->get(ChartObjectData::_PRICE)->toString());
  return 0;
}

void ChartObjectHLine::move (QPoint p)
{
  switch (_status)
  {
    case _MOVE:
    {
      QwtScaleMap map = plot()->canvasMap(QwtPlot::yRight);
      _settings->set(ChartObjectData::_PRICE, new SettingDouble(map.invTransform((double) p.y())));

      plot()->replot();

      Strip strip;
      QString s;
      strip.strip(_settings->get(ChartObjectData::_PRICE)->toDouble(), 4, s);
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectHLine::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place HLine object..."));
  return 0;
}

int ChartObjectHLine::highLow (int, int, double &high, double &low)
{
  double d = _settings->get(ChartObjectData::_PRICE)->toDouble();
  high = d;
  low = d;
  return 0;
}
