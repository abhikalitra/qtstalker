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

#include "HorizontalLine.h"
#include <qpainter.h>
#include <qcolor.h>

HorizontalLine::HorizontalLine (QString indicator, QString name, QString value)
{
  settings.set("Type", "Horizontal Line", Setting::None);
  settings.set(tr("Value"), value, Setting::Date);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (Scaler &scaler, QPixmap &buffer, int, int)
{
  QPainter painter;
  painter.begin(&buffer);

  int y = scaler.convertToY(settings.getFloat(tr("Value")));

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  painter.drawLine (0, y, buffer.width(), y);
  painter.drawText(0, y - 1, settings.getData(tr("Value")), -1);

  painter.end();
}


