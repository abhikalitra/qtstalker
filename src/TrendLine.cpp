/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "TrendLine.h"
#include <qpainter.h>
#include <qcolor.h>

TrendLine::TrendLine (BarData *d, QString indicator, QString name, QString date, QString value,
                      QString date2, QString value2)
{
  data = d;
  settings.set("Type", "TrendLine", Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set(tr("Start Bar"), tr("Close"), Setting::InputField);
  settings.set(tr("End Bar"), tr("Close"), Setting::InputField);
  settings.set(tr("Use Bar"), tr("False"), Setting::Bool);
  settings.set("Start Date", date, Setting::None);
  settings.set("Start Value", value, Setting::None);
  settings.set("End Date", date2, Setting::None);
  settings.set("End Value", value2, Setting::None);
}

TrendLine::~TrendLine ()
{
}

void TrendLine::draw (Scaler &scaler, QPixmap &buffer, int x, int x2)
{
  QPainter painter;
  painter.begin(&buffer);

  int y;
  if (! settings.getData(tr("Use Bar")).compare(tr("True")))
  {
    QString s = settings.getData(tr("Start Bar"));
    QDateTime dt = QDateTime::fromString(settings.getDateTime("Start Date"), Qt::ISODate);
    int i = data->getX(dt);

    while (1)
    {
      if (! s.compare(tr("Open")))
      {
        y = scaler.convertToY(data->getOpen(i));
	break;
      }

      if (! s.compare(tr("High")))
      {
        y = scaler.convertToY(data->getHigh(i));
	break;
      }

      if (! s.compare(tr("Low")))
      {
        y = scaler.convertToY(data->getLow(i));
	break;
      }

      if (! s.compare(tr("Close")))
      {
        y = scaler.convertToY(data->getClose(i));
	break;
      }

      return;
    }
  }
  else
    y = scaler.convertToY(settings.getFloat(tr("Start Value")));

  int y2;
  if (! settings.getData(tr("Use Bar")).compare(tr("True")))
  {
    QString s = settings.getData(tr("End Bar"));
    QDateTime dt = QDateTime::fromString(settings.getDateTime("End Date"), Qt::ISODate);
    int i = data->getX(dt);

    while (1)
    {
      if (! s.compare(tr("Open")))
      {
        y2 = scaler.convertToY(data->getOpen(i));
	break;
      }

      if (! s.compare(tr("High")))
      {
        y2 = scaler.convertToY(data->getHigh(i));
	break;
      }

      if (! s.compare(tr("Low")))
      {
        y2 = scaler.convertToY(data->getLow(i));
	break;
      }

      if (! s.compare(tr("Close")))
      {
        y2 = scaler.convertToY(data->getClose(i));
	break;
      }

      return;
    }
  }
  else
    y2 = scaler.convertToY(settings.getFloat(tr("End Value")));

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  painter.drawLine (x, y, x2, y2);

  int ydiff = y - y2;
  int xdiff = x2 - x;
  while (x2 < buffer.width())
  {
    x = x2;
    y = y2;
    x2 = x2 + xdiff;
    y2 = y2 - ydiff;
    painter.drawLine (x, y, x2, y2);
  }

  painter.end();
}

QString TrendLine::getDate ()
{
  return settings.getDateTime(tr("Start Date"));
}

QString TrendLine::getDate2 ()
{
  return settings.getDateTime(tr("End Date"));
}

