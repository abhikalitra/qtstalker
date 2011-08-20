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


#include "ChartObjectText.h"
#include "Globals.h"
#include "SettingDateTime.h"
#include "SettingInteger.h"
#include "SettingDouble.h"
#include "SettingColor.h"
#include "SettingString.h"
#include "SettingFont.h"

#include <QDebug>
#include <QPolygon>
#include <QSettings>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectText::ChartObjectText ()
{
  _type = "Text";

  SettingString *ss = new SettingString(QString("Text"));
  ss->setKey("TEXT");
  _settings->set(ss);

  SettingFont *sf = new SettingFont(QFont());
  sf->setKey("FONT");
  _settings->set(sf);

  SettingDateTime *dt = new SettingDateTime(QDateTime::currentDateTime());
  dt->setKey("DATE");
  _settings->set(dt);

  SettingDouble *sd = new SettingDouble(0.0);
  sd->setKey("PRICE");
  _settings->set(sd);

  SettingColor *sc = new SettingColor(QColor(Qt::red));
  sc->setKey("COLOR");
  _settings->set(sc);

  SettingInteger *si = new SettingInteger(0, 0, 1, 99, 0);
  si->setKey("Z");
  _settings->set(si);
}

void ChartObjectText::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get("DATE")->getDateTime()));

  int y = yMap.transform(_settings->get("PRICE")->getDouble());

  p->setPen(_settings->get("COLOR")->getColor());

  p->setFont(_settings->get("FONT")->getFont());

  p->drawText(x, y, _settings->get("TEXT")->getString());

  QFontMetrics fm = p->fontMetrics();

  _selectionArea.clear();

  _selectionArea.append(QRegion(x,
		                y - fm.height(),
		                fm.width(_settings->get("TEXT")->getString(), -1),
		                fm.height(),
		                QRegion::Rectangle));

  if (_selected)
  {
    _grabHandles.clear();

    _grabHandles.append(QRegion(x - _handleWidth - 1,
		                y - (fm.height() / 2),
		                _handleWidth,
		                _handleWidth,
		                QRegion::Rectangle));

    p->fillRect(x - _handleWidth - 1,
		y - (fm.height() / 2),
		_handleWidth,
		_handleWidth,
		_settings->get("COLOR")->getColor());
  }
}

void ChartObjectText::move (QPoint p)
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

      map = plot()->canvasMap(QwtPlot::yRight);
      set = _settings->get("PRICE");
      set->setDouble(map.invTransform((double) p.y()));

      plot()->replot();

      QString s = _settings->get("DATE")->toString() + " " + _settings->get("PRICE")->toString();
      g_parent->statusBar()->showMessage(s);

      _modified++;
      break;
    }
    default:
      break;
  }
}

int ChartObjectText::create ()
{
  _status = _MOVE;
  setSelected(TRUE);
  g_parent->statusBar()->showMessage(QObject::tr("Place Text object..."));
  return 0;
}

int ChartObjectText::info (Message &info)
{
  info.insert(QObject::tr("Type"), _type);

  QDateTime dt = _settings->get("DATE")->getDateTime();
  info.insert("D", dt.toString("yyyy-MM-dd"));
  info.insert("T", dt.toString("HH:mm:ss"));

  info.insert(QObject::tr("Price"), _settings->get("PRICE")->toString());
  info.insert(QObject::tr("Text"), _settings->get("TEXT")->getString());

  return 0;
}
