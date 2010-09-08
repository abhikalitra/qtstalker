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


#include "ChartObjectSell.h"
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectSellDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectSell::ChartObjectSell ()
{
  _settings.type = (int) ChartObject::_Sell;

  Config config;
  config.getData(Config::DefaultChartObjectSellColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectSellColor, _settings.color);
  }

  setYAxis(QwtPlot::yRight);
}

int ChartObjectSell::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectSell::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings.date));

  int y = yMap.transform(_settings.price);

  QPolygon arrow;
  arrow.putPoints(0, 7, x, y,
                  x + 5, y - 5,
                  x + 2, y - 5,
                  x + 2, y - 11,
                  x - 2, y - 11,
                  x - 2, y - 5,
                  x - 5, y - 5);
  p->setBrush(_settings.color);

  p->drawPolygon(arrow, Qt::OddEvenFill);

  _selectionArea.clear();

  _selectionArea.append(QRegion(arrow));


/*
  if (_selected)
  {
    clearGrabHandles();

    setGrabHandle(new QRegion(x - (_handleWidth / 2),
                  y - _handleWidth,
                  _handleWidth,
                  _handleWidth,
                  QRegion::Rectangle));

    painter.fillRect(x - (_handleWidth / 2),
                     y + (_handleWidth / 2),
                     _handleWidth,
                     _handleWidth,
                     _color);
  }
*/
}

void ChartObjectSell::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Sell"));
  info.setData(QObject::tr("D"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("Price"), _settings.price);
}

ChartObjectDialog * ChartObjectSell::dialog ()
{
  ChartObjectSellDialog *dialog = new ChartObjectSellDialog;
  dialog->setSettings(_settings);
  return dialog;
}

int ChartObjectSell::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<PRICE>,<COLOR>
  //  0    1      2      3       4

  if (l.count() != 5)
  {
    qDebug() << "ChartObjectSell::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectSell::CUS: invalid date" << l[2];
    return 1;
  }

  bool ok;
  _settings.price = l[3].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectSell::CUS: invalid price" << l[3];
    return 1;
  }

  _settings.color.setNamedColor(l[4]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectSell::CUS: invalid color" << l[4];
    return 1;
  }

  return 0;
}

int ChartObjectSell::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);

  if (x < start || x > end)
    return 0;

  h = _settings.price;
  l = _settings.price;

  return 1;
}

