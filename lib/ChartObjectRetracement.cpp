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


#include "ChartObjectRetracement.h"
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectRetracementDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectRetracement::ChartObjectRetracement ()
{
  _settings.type = (int) ChartObject::_Retracement;

  Config config;
  config.getData(Config::DefaultChartObjectRetracementColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectRetracementColor, _settings.color);
  }

  _settings.line1 = config.getDouble(Config::DefaultChartObjectRetracementLine1);
  if (! _settings.line1)
  {
    _settings.line1 = 0.382;
    config.setData(Config::DefaultChartObjectRetracementLine1, _settings.line1);
  }

  _settings.line2 = config.getDouble(Config::DefaultChartObjectRetracementLine2);
  if (! _settings.line2)
  {
    _settings.line2 = 0.5;
    config.setData(Config::DefaultChartObjectRetracementLine2, _settings.line2);
  }

  _settings.line3 = config.getDouble(Config::DefaultChartObjectRetracementLine3);
  if (! _settings.line3)
  {
    _settings.line3 = 0.618;
    config.setData(Config::DefaultChartObjectRetracementLine3, _settings.line3);
  }

  _settings.line4 = config.getDouble(Config::DefaultChartObjectRetracementLine4);
  _settings.line5 = config.getDouble(Config::DefaultChartObjectRetracementLine5);
  _settings.line6 = config.getDouble(Config::DefaultChartObjectRetracementLine6);

  setYAxis(QwtPlot::yRight);
}

int ChartObjectRetracement::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectRetracement::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const
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

  p->setPen(_settings.color);

  QPolygon array;
  
  _selectionArea.clear();

  QList<double> lineList;
  lineList.append(_settings.line1);
  lineList.append(_settings.line2);
  lineList.append(_settings.line3);
  lineList.append(_settings.line4);
  lineList.append(_settings.line5);
  lineList.append(_settings.line6);

  int loop;
  for (loop = 0; loop < lineList.count(); loop++)
  {
    double td = lineList[loop];
    if (td != 0)
    {
      double range = _settings.high - _settings.low;
      double r = 0;
      if (td < 0)
        r = _settings.low + (range * td);
      else
      {
        if (td > 0)
          r = _settings.low + (range * td);
        else
        {
          if (td < 0)
            r = _settings.high;
          else
            r = _settings.low;
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
  int y = yMap.transform(_settings.low);
  p->drawLine (x, y, x2, y);
  p->drawText(x, y - 1, "0% - " + QString::number(_settings.low));

  // store the selectable area the low line occupies
  array.putPoints(0, 4, x, y - 4, x, y + 4, x2, y + 4, x2, y - 4);
  _selectionArea.append(QRegion(array));

  // draw the high line
  int y2 = yMap.transform(_settings.high);
  p->drawLine (x, y2, x2, y2);
  p->drawText(x, y2 - 1, "100% - " + QString::number(_settings.high));

  // store the selectable area the high line occupies
  array.putPoints(0, 4, x, y2 - 4, x, y2 + 4, x2, y2 + 4, x2, y2 - 4);
  _selectionArea.append(QRegion(array));

/*
  if (_selected)
  {
    clearGrabHandles();

    //top left corner
    y = scaler.convertToY(_high);
    setGrabHandle(new QRegion(x, y - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x,
		     y - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);

    //bottom right corner
    x2 = (dateBars.getX(_date2) * pd.barSpacing) - (pd.startIndex * pd.barSpacing);
    y2 = scaler.convertToY(_low);
    setGrabHandle(new QRegion(x2, y2 - (_handleWidth / 2),
		  _handleWidth,
		  _handleWidth,
		  QRegion::Rectangle));

    painter.fillRect(x2,
		     y2 - (_handleWidth / 2),
		     _handleWidth,
		     _handleWidth,
		     _color);
  }
*/
}

void ChartObjectRetracement::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("Retracement"));
  info.setData(QObject::tr("SD"), _settings.date.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ST"), _settings.date.toString("HH:mm:ss"));
  info.setData(QObject::tr("ED"), _settings.date2.toString("yyyy-MM-dd"));
  info.setData(QObject::tr("ET"), _settings.date2.toString("HH:mm:ss"));
  info.setData(QObject::tr("High"), _settings.high);
  info.setData(QObject::tr("Low"), _settings.low);
  info.setData(QObject::tr("Level 1"), _settings.line1);
  info.setData(QObject::tr("Level 2"), _settings.line2);
  info.setData(QObject::tr("Level 3"), _settings.line3);
  info.setData(QObject::tr("Level 4"), _settings.line4);
  info.setData(QObject::tr("Level 5"), _settings.line5);
  info.setData(QObject::tr("Level 6"), _settings.line6);
}

ChartObjectDialog * ChartObjectRetracement::dialog ()
{
  ChartObjectRetracementDialog *dialog = new ChartObjectRetracementDialog;
  dialog->setSettings(_settings);
  return dialog;
}

int ChartObjectRetracement::highLow (int start, int end, double &h, double &l)
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = dsd->x(_settings.date);
  int x2 = dsd->x(_settings.date2);

  if ((x >= start && x <= end) || ((x2 >= start && x2 <= end)))
  {
    h = _settings.high;
    l = _settings.low;
    return 1;
  }

  return 0;
}

int ChartObjectRetracement::CUS (QStringList &l)
{
  // CO,<TYPE>,<DATE>,<DATE2>,<HIGH>,<LOW>,<PERCENTAGE>,<COLOR>
  //  0    1      2      3      4      5        6          7

  if (l.count() != 8)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid parm count" << l.count();
    return 1;
  }

  _settings.date = QDateTime::fromString(l[2], Qt::ISODate);
  if (! _settings.date.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid start date" << l[2];
    return 1;
  }

  _settings.date2 = QDateTime::fromString(l[3], Qt::ISODate);
  if (! _settings.date2.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid end date" << l[3];
    return 1;
  }

  bool ok;
  _settings.high = l[4].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid high" << l[4];
    return 1;
  }

  _settings.low = l[5].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid low" << l[5];
    return 1;
  }

  _settings.line1 = l[6].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid retracement" << l[6];
    return 1;
  }

  _settings.color.setNamedColor(l[7]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectRetracement::CUS: invalid color" << l[7];
    return 1;
  }

  _settings.line2 = 0;
  _settings.line3 = 0;
  _settings.line4 = 0;
  _settings.line5 = 0;
  _settings.line6 = 0;

  return 0;
}

