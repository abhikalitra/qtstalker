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
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectVLineDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectVLine::ChartObjectVLine ()
{
  _settings.type = (int) ChartObject::_VLine;

  Config config;
  config.getData(Config::DefaultChartObjectVLineColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectVLineColor, _settings.color);
  }

  setYAxis(QwtPlot::yRight);
}

int ChartObjectVLine::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings.date));

  p->setPen(_settings.color);

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

/*  
  if (_selected)
  {
    clearGrabHandles();
    int t = (int) pd.buffer.height() / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      setGrabHandle(new QRegion(x - (_handleWidth / 2),
		    t * loop,
		    _handleWidth,
		    _handleWidth,
		    QRegion::Rectangle));

      painter.fillRect(x - (_handleWidth / 2),
		       t * loop,
		       _handleWidth,
		       _handleWidth,
		       _color);
    }
  }
*/
}

void ChartObjectVLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("VLine"));
  info.setData(QObject::tr("D"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("T"), _settings.date.toString("HH:mm:ss"));
}

ChartObjectDialog * ChartObjectVLine::dialog ()
{
  ChartObjectVLineDialog *dialog = new ChartObjectVLineDialog;
  dialog->setSettings(_settings);
  return dialog;
}

void ChartObjectVLine::highLow (double &, double &)
{
}

int ChartObjectVLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<COLOR>
  //  0    1      2      3

  if (l.count() != 4)
  {
    qDebug() << "ChartObjectVLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid date" << l[2];
    return 1;
  }

  _settings.color.setNamedColor(l[3]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectVLine::CUS: invalid color" << l[3];
    return 1;
  }

  return 0;
}

