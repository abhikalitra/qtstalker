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

#include "VerticalLine.h"
#include "PrefDialog.h"
#include <qpainter.h>

VerticalLine::VerticalLine (QPixmap *p, QString indicator, QString n, BarDate d)
{
  buffer = p;
  type = ChartObject::VerticalLine;
  plot = indicator;
  name = n;
  date = d;
  color.setNamedColor("white");
  
  menu->insertItem(tr("Edit Vertical Line"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete Vertical Line"), this, SLOT(remove()));
}

VerticalLine::~VerticalLine ()
{
}

void VerticalLine::draw (int x, int)
{
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(color);

  painter.drawLine (x, 0, x, buffer->height());
  
  QRegion r(x, 0, 1, buffer->height(), QRegion::Rectangle);
  area = r;
  
  if (status)
  {
    int t = (int) buffer->height() / 4;
    painter.fillRect(x - 3, 0, 6, 6, color);
    painter.fillRect(x - 3, t, 6, 6, color);
    painter.fillRect(x - 3, t * 2, 6, 6, color);
    painter.fillRect(x - 3, t * 3, 6, 6, color);
    painter.fillRect(x - 3, t * 4, 6, 6, color);
  }

  painter.end();
}

void VerticalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Vertical Line"));
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

void VerticalLine::move (BarDate d, double)
{
  date = d;
  saveFlag = TRUE;
  emit signalDraw();
  emit message(d.getDateString(TRUE));
}

Setting * VerticalLine::getSettings ()
{
  Setting *set = new Setting;
  set->setData("Date", date.getDateTimeString(FALSE));
  set->setData("Color", color.name());
  set->setData("Plot", plot);
  set->setData("Name", name);
  set->setData("ObjectType", QString::number(type));
  return set;
}

void VerticalLine::setSettings (Setting *set)
{
  date.setDate(set->getData("Date"));
  color.setNamedColor(set->getData("Color"));
  plot = set->getData("Plot");
  name = set->getData("Name");
  type = (ChartObject::ObjectType) set->getInt("ObjectType");
}

