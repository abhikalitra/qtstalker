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
#include "PrefDialog.h"
#include <qpainter.h>
#include <qcolor.h>
#include <qpointarray.h>

SellArrow::SellArrow (Scaler *s, QPixmap *p, QString indicator, QString name, QString date, QString value)
{
  scaler = s;
  buffer = p;
  
  settings.set("Type", "Sell Arrow", Setting::None);
  settings.set("Date", date, Setting::None);
  settings.set("Value", value, Setting::None);
  settings.set(tr("Color"), "red", Setting::Color);
  settings.set("Plot", indicator, Setting::None);
  settings.set("Name", name, Setting::None);
  settings.set("ObjectType", QString::number(ChartObject::SellArrow), Setting::None);
}

SellArrow::~SellArrow ()
{
}

void SellArrow::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);

  int y = scaler->convertToY(settings.getFloat("Value"));

  QColor color(settings.getData(tr("Color")));

  QPointArray array;
  array.setPoints(7, x, y,
                  x + 5, y - 5,
	          x + 2, y - 5,
	          x + 2, y - 11,
	          x - 2, y - 11,
	          x - 2, y - 5,
                  x - 5, y - 5);
  painter.setBrush(color);
  painter.drawPolygon(array, TRUE, 0, -1);
  
  QRegion r(array, FALSE);
  area = r;
  
  if (status)
  {
    painter.setBrush(NoBrush);
    painter.setPen(color);
    QRect r = area.boundingRect();
    painter.drawRect(r.topLeft().x() - 2, r.topLeft().y() - 2, r.width() + 4, r.height() + 4);
  }

  painter.end();
}

QString SellArrow::getDate ()
{
  return settings.getDateTime("Date");
}

void SellArrow::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Sell Arrow"));
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

void SellArrow::move (QString d, QString v)
{
  settings.setData("Date", d);
  settings.setData("Value", v);
  saveFlag = TRUE;
  emit signalDraw();
}

