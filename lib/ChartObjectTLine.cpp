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


#include "ChartObjectTLine.h"
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectTLineDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectTLine::ChartObjectTLine ()
{
  _fieldList << QObject::tr("Open") << QObject::tr("High") << QObject::tr("Low") << QObject::tr("Close");

  _settings.type = (int) ChartObject::_TLine;

  Config config;
  config.getData(Config::DefaultChartObjectTLineColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::green);
    config.setData(Config::DefaultChartObjectTLineColor, _settings.color);
  }

  QString s;
  config.getData(Config::DefaultChartObjectTLineExtend, s);
  if (s.isEmpty())
  {
    _settings.extend = 1;
    config.setData(Config::DefaultChartObjectTLineExtend, _settings.extend);
  }
  else
    _settings.extend = s.toInt();

  setYAxis(QwtPlot::yRight);
}

int ChartObjectTLine::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectTLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings.date));
  if (x == -1)
    return;

  QDateTime dt = _settings.date2;
  if (_settings.extend)
    dsd->date(dsd->count() - 1, dt);

  int x2 = xMap.transform(dsd->x(dt));
  if (x2 == -1)
    return;

  int y = yMap.transform(_settings.price);
  int y2 = yMap.transform(_settings.price2);

  p->setPen(_settings.color);

  p->drawLine (x, y, x2, y2);

  // store the selectable area the line occupies
  _selectionArea.clear();
  
  QPolygon array;
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y2 + 4, x2, y2 - 4);
  _selectionArea.append(QRegion(array));

/*  
  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(tx,
		  ty - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(tx,
		     ty - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);

    setGrabHandle(new QRegion(tx2,
		  ty2 - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(tx2,
		     ty2 - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }
*/
}

void ChartObjectTLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("TLine"));
  info.setData(QObject::tr("SD"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ST"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("ED"), _settings.date2.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ET"), _settings.date2.toString("HH:mm:ss"));
  info.setData(QObject::tr("SP"), _settings.price);
  info.setData(QObject::tr("EP"), _settings.price2);
}

ChartObjectDialog * ChartObjectTLine::dialog ()
{
  ChartObjectTLineDialog *dialog = new ChartObjectTLineDialog;
  dialog->setSettings(_settings);
  return dialog;
}

int ChartObjectTLine::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);
  int x2 = dsd->x(_settings.date2);

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = -99999999.0;
    l = 99999999.0;

    if (_settings.price > h)
      h = _settings.price;

    if (_settings.price < l)
      l = _settings.price;

    if (_settings.price2 > h)
      h = _settings.price2;

    if (_settings.price2 < l)
      l = _settings.price2;
    
    return 1;
  }

  return 0;
}

int ChartObjectTLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<DATE2>,<PRICE2>,<COLOR>
  //  0    1      2      3       4       5        6

  if (l.count() != 7)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start date" << l[2];
    return 1;
  }

  bool ok;
  _settings.price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid start price" << l[3];
    return 1;
  }

  _settings.date2 = QDateTime::fromString(l[4], Qt::ISODate);
  if (! _settings.date2.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end date" << l[4];
    return 1;
  }

  _settings.price2 = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectTLine::CUS: invalid end price" << l[5];
    return 1;
  }

  _settings.color.setNamedColor(l[6]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectTLine::CUS: invalid color" << l[6];
    return 1;
  }

  return 0;
}

