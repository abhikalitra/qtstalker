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

#include "SellArrow.h"
#include <qpainter.h>
#include <qcolor.h>
#include <qpointarray.h>

SellArrow::SellArrow (QString indicator, QString name, QString date, QString value)
{
  settings.set("Type", "Sell Arrow", Setting::None);
  settings.set(tr("Date"), date, Setting::Date);
  settings.set(tr("Value"), value, Setting::Float);
  settings.set(tr("Color"), "red", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
}

SellArrow::~SellArrow ()
{
}

void SellArrow::draw (Scaler &scaler, QPixmap &buffer, int x, int)
{
  QPainter painter;
  painter.begin(&buffer);

  int y = scaler.convertToY(settings.getFloat(tr("Value")));

  QColor color(settings.getData(tr("Color")));

  QPointArray array;
  array.setPoints(7, x, y,
                  x + 4, y - 4,
	          x + 1, y - 4,
	          x + 1, y - 10,
	          x - 1, y - 10,
	          x - 1, y - 4,
                  x - 4, y - 4);
  painter.setBrush(color);
  painter.drawPolygon(array, TRUE, 0, -1);

  painter.end();
}

QString SellArrow::getDate ()
{
  return settings.getDateTime(tr("Date"));
}

