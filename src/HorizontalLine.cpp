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
#include <qpointarray.h>

HorizontalLine::HorizontalLine (Scaler *s, QPixmap *p, QString indicator, QString n, double v)
{
  scaler = s;
  buffer = p;
  type = ChartObject::HorizontalLine;
  plot = indicator;
  name = n;
  value = v;
  
  menu->insertItem(tr("&Edit Horizontal Line"), this, SLOT(prefDialog()));
  menu->insertItem(tr("&Move Horizontal Line"), this, SLOT(moveObject()));
  menu->insertItem(tr("&Delete Horizontal Line"), this, SLOT(remove()));
  
  loadDefaults("HorizontalLine");
}

HorizontalLine::~HorizontalLine ()
{
}

void HorizontalLine::draw (int, int)
{
  int y = scaler->convertToY(value);
  ty = y;
  
  // if value is off chart then don't draw it
  if (value < scaler->getLow())
    return;
  
  QPainter painter;
  painter.begin(buffer);
  painter.setPen(color);

  painter.drawLine (0, y, buffer->width(), y);
  painter.drawText(0, y - 1, QString::number(value), -1);
  
  selectionArea.clear();
  QPointArray array;
  array.putPoints(0, 4, 0, y - 4, 0, y + 4, buffer->width(), y + 4, buffer->width(), y - 4);
  selectionArea.append(new QRegion(array));
  
  if (status)
  {
    grabHandles.clear();
    int t = (int) buffer->width() / 4;
    
    grabHandles.append(new QRegion(0,
             			   y - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(0, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  
    grabHandles.append(new QRegion(t,
             			   y - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(t, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    grabHandles.append(new QRegion(t * 2,
             			   y - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(t * 2, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    grabHandles.append(new QRegion(t * 3,
             			   y - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(t * 3, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
    
    grabHandles.append(new QRegion(t * 4,
             			   y - (HANDLE_WIDTH / 2),
				   HANDLE_WIDTH,
				   HANDLE_WIDTH,
				   QRegion::Rectangle));
    painter.fillRect(t * 4, y - (HANDLE_WIDTH / 2), HANDLE_WIDTH, HANDLE_WIDTH, color);
  }

  painter.end();
}

void HorizontalLine::prefDialog ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Horizontal Line"));
  dialog->createPage (tr("Details"));
  dialog->addColorItem(tr("Color"), tr("Details"), color);
  dialog->addCheckItem(tr("Set Default"), tr("Details"), FALSE);
  
  int rc = dialog->exec();
  
  if (rc == QDialog::Accepted)
  {
    color = dialog->getColor(tr("Color"));
    
    bool f = dialog->getCheck(tr("Set Default"));
    if (f)
      saveDefaults("HorizontalLine");
    
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

