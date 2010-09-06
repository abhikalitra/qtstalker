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
#include "Config.h"
#include "ChartObjectDataBase.h"
#include "ChartObjectHLineDialog.h"
#include "DateScaleDraw.h"

#include <QDebug>
#include <QtSql>
#include <qwt_plot.h>

ChartObjectHLine::ChartObjectHLine ()
{
  _settings.type = (int) ChartObject::_HLine;

  Config config;
  config.getData(Config::DefaultChartObjectHLineColor, _settings.color);
  if (! _settings.color.isValid())
  {
    _settings.color = QColor(Qt::red);
    config.setData(Config::DefaultChartObjectHLineColor, _settings.color);
  }

  setYAxis(QwtPlot::yRight);
}

int ChartObjectHLine::rtti () const
{
  return Rtti_PlotUserItem;
}

void ChartObjectHLine::draw (QPainter *p, const QwtScaleMap &, const QwtScaleMap &yMap, const QRect &) const
{
  p->setPen(_settings.color);

  int y = yMap.transform(_settings.price);

  // test start
  QString s = QString::number(_settings.price);
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

/*  
  if (_selected)
  {
    clearGrabHandles();
    int t = (int) (pd.buffer.width() - pd.scaleWidth) / 4;
    int loop;
    for (loop = 0; loop < 5; loop++)
    {
      setGrabHandle(new QRegion(t * loop,
                    y - (_handleWidth / 2),
                    _handleWidth,
                    _handleWidth,
                    QRegion::Rectangle));

      painter.fillRect(t * loop,
                       y - (_handleWidth / 2),
                       _handleWidth,
                       _handleWidth,
                       _color);
    }
  }
*/
}

void ChartObjectHLine::info (Setting &info)
{
  info.setData(QObject::tr("Type"), QObject::tr("HLine"));
  info.setData(QObject::tr("Price"), _settings.price);
}

ChartObjectDialog * ChartObjectHLine::dialog ()
{
  ChartObjectHLineDialog *dialog = new ChartObjectHLineDialog;
  dialog->setSettings(_settings);
  return dialog;
}

void ChartObjectHLine::highLow (double &h, double &l)
{
  h = _settings.price;
  l = _settings.price;
}

int ChartObjectHLine::CUS (QStringList &l)
{
  // CO,<TYPE>,<PRICE>,<COLOR>
  //  0    1      2       3

  if (l.count() != 4)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid parm count" << l.count();
    return 1;
  }

  bool ok;
  _settings.price = l[2].toDouble(&ok);
  if (! ok)
  {
    qDebug() << "ChartObjectHLine::CUS: invalid price" << l[2];
    return 1;
  }

  _settings.color.setNamedColor(l[3]);
  if (! _settings.color.isValid())
  {
    qDebug() << "ChartObjectHLine::CUS: invalid color" << l[3];
    return 1;
  }

  return 0;
}

