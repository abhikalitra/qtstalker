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

#include "ChartObject.h"
#include <qcursor.h>

ChartObject::ChartObject ()
{
  data = 0;
  scaler = 0;
  buffer = 0;
  saveFlag = FALSE;
  status = FALSE;
  
  menu = new QPopupMenu();
  menu->insertItem(tr("Edit"), this, SLOT(prefDialog()));
  menu->insertItem(tr("Delete"), this, SLOT(remove()));
}

ChartObject::~ChartObject ()
{
  delete menu;
}

void ChartObject::draw (int, int)
{
}

QString ChartObject::getDate ()
{
  return QString();
}

QString ChartObject::getDate2 ()
{
  return QString();
}

void ChartObject::setData (QString d)
{
  settings.clear();
  settings.parse(d);
}

QString ChartObject::getData (QString d)
{
  return settings.getData(d);
}

QString ChartObject::getString ()
{
  return settings.getString();
}

void ChartObject::prefDialog ()
{
}

void ChartObject::showMenu ()
{
  menu->exec(QCursor::pos());
}

void ChartObject::remove ()
{
  emit signalDeleteChartObject(settings.getData("Name"));
}

bool ChartObject::isClicked (int x, int y)
{
  return area.contains(QPoint(x,y));
}

void ChartObject::unselect ()
{
  status = FALSE;
  emit signalDraw();
}

void ChartObject::move (QString, QString)
{
}

bool ChartObject::getSaveFlag ()
{
  return saveFlag;
}

void ChartObject::selected (int x, int y)
{
  if (status)
    return;
    
  if (! area.contains(QPoint(x,y)))
    return;
    
  status = TRUE;
  emit signalDraw();
  emit signalChartObjectSelected(this);
}


