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
#include "PrefDialog.h"
#include <qpainter.h>
#include <qcolor.h>

HorizontalLine::HorizontalLine (Scaler *s, QPixmap *p, QString indicator, QString name, QString value)
{
  scaler = s;
  buffer = p;
  
  settings.set("Type", "Horizontal Line", Setting::None);
  settings.set("Value", value, Setting::None);
  settings.set(tr("Color"), "white", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set("ObjectType", QString::number(ChartObject::HorizontalLine), Setting::None);
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (int, int)
{
  QPainter painter;
  painter.begin(buffer);

  int y = scaler->convertToY(settings.getFloat("Value"));

  QColor color(settings.getData(tr("Color")));
  painter.setPen(color);

  painter.drawLine (0, y, buffer->width(), y);
  painter.drawText(0, y - 1, settings.getData("Value"), -1);
  
  QRegion r(0, y, buffer->width(), 1, QRegion::Rectangle);
  area = r;
  
  if (status)
  {
    int t = (int) buffer->width() / 4;
    painter.fillRect(0, y - 3, 6, 6, color);
    painter.fillRect(t, y - 3, 6, 6, color);
    painter.fillRect(t * 2, y - 3, 6, 6, color);
    painter.fillRect(t * 3, y - 3, 6, 6, color);
    painter.fillRect(t * 4, y - 3, 6, 6, color);
  }

  painter.end();
}

void HorizontalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Horizontal Line"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, QColor(settings.getData(tr("Color"))));
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Color"));
    settings.setData(tr("Color"), color.name());
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void HorizontalLine::move (QString, QString v)
{
  settings.setData("Value", v);
  saveFlag = TRUE;
  emit signalDraw();
}

