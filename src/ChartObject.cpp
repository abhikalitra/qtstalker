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
#include <qapplication.h>
#include <qsettings.h>

ChartObject::ChartObject ()
{
  data = 0;
  scaler = 0;
  buffer = 0;
  saveFlag = FALSE;
  status = FALSE;
  font = QApplication::font();
  color.setNamedColor("red");
  grabHandles.setAutoDelete(TRUE);
  selectionArea.setAutoDelete(TRUE);
  
  menu = new QPopupMenu();
}

ChartObject::~ChartObject ()
{
  delete menu;
}

void ChartObject::draw (int, int)
{
}

BarDate ChartObject::getDate ()
{
  return date;
}

BarDate ChartObject::getDate2 ()
{
  return date2;
}

void ChartObject::setSettings (Setting *)
{
}

Setting * ChartObject::getSettings ()
{
  return 0;
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
  emit signalDeleteChartObject(name);
}

void ChartObject::moveObject ()
{
  emit signalMoving();
}

bool ChartObject::handleClicked (int x, int y)
{
  int loop;
  bool rc = FALSE;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    rc = r->contains(QPoint(x,y));
    if (rc)
      break;
  }
    
  if (rc)
    moveObject();
    
  return rc;
}

void ChartObject::unselect ()
{
  status = FALSE;
  emit signalDraw();
}

void ChartObject::move (BarDate, double)
{
}

bool ChartObject::getSaveFlag ()
{
  return saveFlag;
}

void ChartObject::setSaveFlag (bool d)
{
  saveFlag = d;
}

void ChartObject::selected ()
{
  status = TRUE;
  emit signalDraw();
  emit signalChartObjectSelected(this);
}

bool ChartObject::isSelected (int x, int y)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(QPoint(x,y)))
      return TRUE;
  }
  
  return FALSE;
}

ChartObject::ObjectType ChartObject::getType ()
{
  return type;
}

double ChartObject::getValue ()
{
  return value;
}

double ChartObject::getValue2 ()
{
  return value2;
}

QString ChartObject::getName ()
{
  return name;
}

void ChartObject::loadDefaults (QString key)
{
  QSettings settings;
  
  QString s = "/Qtstalker/Default" + key + "Color";
  s = settings.readEntry(s);
  if (s.length())
    color.setNamedColor(s);

  s = "/Qtstalker/Default" + key + "Font";
  s = settings.readEntry(s);
  if (s.length())
  {
    QStringList l = QStringList::split(",", s, FALSE);
    font = QFont(l[0], l[1].toInt(), l[2].toInt());
  }
}

void ChartObject::saveDefaults (QString key)
{
  QSettings settings;
  
  QString s = "/Qtstalker/Default" + key + "Color";
  settings.writeEntry(s, color.name());
  
  s = "/Qtstalker/Default" + key + "Font";
  settings.writeEntry(s, font.family() + "," +
                      QString::number(font.pointSize()) + "," +
		      QString::number(font.weight()));
}

void ChartObject::keyEvent (QKeyEvent *key)
{
  switch (key->key())
  {
    case Qt::Key_M:
      if (key->state() == Qt::ControlButton)
        moveObject();
      break;
    case Qt::Key_E:
      if (key->state() == Qt::ControlButton)
        prefDialog();
      break;
    case Qt::Key_D:
      if (key->state() == Qt::ControlButton)
        remove();
      break;
    default:
      key->ignore();
      break;
  }
}
