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

HorizontalLine::HorizontalLine (Scaler *s, QPixmap *p, QString indicator, QString n, double v)
{
  scaler = s;
  buffer = p;
  type = ChartObject::HorizontalLine;
  plot = indicator;
  name = n;
  value = v;
  color.setNamedColor("white");
  
  menu->insertItem(tr("Edit Horizontal Line"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete Horizontal Line"), this, SLOT(remove()));
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (int, int)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(color);

  int y = scaler->convertToY(value);

  painter.drawLine (0, y, buffer->width(), y);
  painter.drawText(0, y - 1, QString::number(value), -1);
  
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
  dialog->addColorItem(tr("Color"), 1, color);
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void HorizontalLine::move (BarDate, double v)
{
  value = v;
  saveFlag = TRUE;
  emit signalDraw();
  emit message(QString::number(v));
}

Setting * HorizontalLine::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Value", QString::number(value));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("ObjectType", QString::number(type));
  return set;
}

void HorizontalLine::setSettings (Setting *set)
{
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
}

