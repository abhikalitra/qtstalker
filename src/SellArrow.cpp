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
#include <qpointarray.h>

SellArrow::SellArrow (Scaler *s, QPixmap *p, QString indicator, QString n, BarDate d, double v)
{
  scaler = s;
  buffer = p;
  type = ChartObject::SellArrow;
  plot = indicator;
  name = n;
  date = d;
  value = v;
  
  menu->insertItem(tr("Edit Sell Arrow"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Move Sell Arrow"), this, SLOT(moveObject()));
  menu->insertItem(tr("Delete Sell Arrow"), this, SLOT(remove()));
  
  loadDefaults("SellArrow");
}

SellArrow::~SellArrow ()
{
}

void SellArrow::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);

  int y = scaler->convertToY(value);

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

void SellArrow::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Sell Arrow"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), 1, color);
  dialog->addCheckItem(tr("Set Default"), 1, FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
      saveDefaults("SellArrow");
    
    saveFlag = TRUE;
    emit signalDraw();
  }
  
  delete dialog;
}

void SellArrow::move (BarDate d, double v)
{
  date = d;
  value = v;
  saveFlag = TRUE;
  emit signalDraw();
  
  QString s = d.getDateString(TRUE) + " " + QString::number(v);
  emit message(s);
}

Setting * SellArrow::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Date", date.getDateTimeString(FALSE));
  set->setData("Value", QString::number(value));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("ObjectType", QString::number(type));
  return set;
}

void SellArrow::setSettings (Setting *set)
{
  date.setDate(set->getData("Date"));
  value = set->getFloat("Value");
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
}

