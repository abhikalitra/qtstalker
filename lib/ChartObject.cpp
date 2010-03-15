/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include <QDebug>

ChartObject::ChartObject ()
{
  QString s = "red";
  setData(ParmColor, s);
  setData(ParmSelected, 0);
  setData(ParmSave, 0);
  setData(ParmHigh, 0);
  setData(ParmLow, 0);
}

ChartObject::~ChartObject ()
{
  qDeleteAll(grabHandles);
  qDeleteAll(selectionArea);
}

void ChartObject::setData (Parm k, int d)
{
  QString s = QString::number(d);
  parms.insert((int) k, s);
}

int ChartObject::getInt (Parm k)
{
  int rc = 0;
  if (parms.contains(k))
    rc = parms.value(k).toInt();
  return rc;
}

void ChartObject::setData (Parm k, double d)
{
  QString s = QString::number(d);
  parms.insert((int) k, s);
}

double ChartObject::getDouble (Parm k)
{
  double rc = 0;
  if (parms.contains(k))
    rc = parms.value(k).toDouble();
  return rc;
}

void ChartObject::setData (Parm k, QDateTime &d)
{
  QString s = d.toString(Qt::ISODate);
  parms.insert((int) k, s);
}

void ChartObject::getDate (Parm k, QDateTime &d)
{
  if (parms.contains(k))
    d = QDateTime::fromString(parms.value(k), Qt::ISODate);
}

void ChartObject::setData (Parm k, QString &d)
{
  parms.insert((int) k, d);
}

void ChartObject::getData (Parm k, QString &d)
{
  d.clear();
  if (parms.contains(k))
    d = parms.value(k);
}

void ChartObject::setData (Parm k, QColor &d)
{
  QString s = d.name();
  parms.insert((int) k, s);
}

void ChartObject::getColor (Parm k, QColor &d)
{
  if (parms.contains(k))
    d.setNamedColor(parms.value(k));
}

void ChartObject::setData (Parm k, QFont &d)
{
  QString s = d.family() + "," +
              QString::number(d.pointSize()) + "," +
              QString::number(d.weight()) + "," +
              QString::number(d.italic ()) + "," +
              QString::number(d.bold ());
  parms.insert((int) k, s);
}

void ChartObject::getFont (Parm k, QFont &d)
{
  if (parms.contains(k))
  {
    QStringList l = parms.value(k).split(",");
    d.setFamily(l[0]);
    d.setPointSize(l[1].toInt());
    d.setWeight(l[2].toInt());
    d.setItalic(l[3].toInt());
    d.setBold(l[4].toInt());
  }
}

void ChartObject::message (QString &d)
{
  emit signalMessage(d);
}

void ChartObject::clearGrabHandles ()
{
  qDeleteAll(grabHandles);
  grabHandles.clear();
}

void ChartObject::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void ChartObject::clearSelectionArea ()
{
  qDeleteAll(selectionArea);
  selectionArea.clear();
}

void ChartObject::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

int ChartObject::isSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) selectionArea.count(); loop++)
  {
    QRegion *r = selectionArea.at(loop);
    if (r->contains(point))
      return 1;
  }

  return 0;
}

int ChartObject::isGrabSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) grabHandles.count(); loop++)
  {
    QRegion *r = grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }

  return 0;
}

