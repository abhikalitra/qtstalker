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
#include "DateScaleDraw.h"
#include "ChartObjectData.h"

#include <QDebug>
#include <QPolygon>
#include <QStatusBar>
#include <qwt_plot.h>

ChartObjectVLine::ChartObjectVLine ()
{
  _settings->set(ChartObjectData::_TYPE, QVariant(QString("VLine")));
  _settings->set(ChartObjectData::_DATE, QVariant(QDateTime::currentDateTime()));
  _settings->set(ChartObjectData::_COLOR, QVariant(QString("red")));
  _settings->set(ChartObjectData::_Z, QVariant(1));
  _settings->set(ChartObjectData::_PEN, QVariant(1));
}

void ChartObjectVLine::draw (QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &, const QRect &) const
{
  DateScaleDraw *dsd = (DateScaleDraw *) plot()->axisScaleDraw(QwtPlot::xBottom);
  int x = xMap.transform(dsd->x(_settings->get(ChartObjectData::_DATE).toDateTime()));

  p->setPen(QColor(_settings->get(ChartObjectData::_COLOR).toString()));

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
		  QColor(_settings->get(ChartObjectData::_COLOR).toString()));
    }
  }
}

int ChartObjectVLine::info (Message &info)
{
  info.insert(QObject::tr("Type"), _settings->get(ChartObjectData::_TYPE).toString());

  QDateTime dt = _settings->get(ChartObjectData::_DATE).toDateTime();
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
      _settings->set(ChartObjectData::_DATE, QVariant(dt));

      plot()->replot();

      QString s = _settings->get(ChartObjectData::_DATE).toString();
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

DataDialog * ChartObjectVLine::dialog (QWidget *p)
{
  DataDialog *dialog = new DataDialog(p, _settings);

  QStringList l;
  l << "QtStalker" + g_session + ":" << QObject::tr("Edit VLine");
  dialog->setWindowTitle(l.join(" "));

  dialog->addTab(QObject::tr("Settings"));
  int tab = 0;

  dialog->setDateTime(tab,
                      QString::number(ChartObjectData::_DATE),
                      QObject::tr("Date"),
                      _settings->get(ChartObjectData::_DATE).toDateTime(),
                      QString());

  dialog->setColor(tab,
                   QString::number(ChartObjectData::_COLOR),
                   QObject::tr("Color"),
                   QColor(_settings->get(ChartObjectData::_COLOR).toString()),
                   QString());

  dialog->setInteger(tab,
                     QString::number(ChartObjectData::_Z),
                     QString("Z"),
                     _settings->get(ChartObjectData::_Z).toInt(),
                     99,
                     -1,
                     QString());

  return dialog;
}
