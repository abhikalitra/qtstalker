/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "BaseCO.h"

#include <QObject>

BaseCO::BaseCO ()
{
  color.setNamedColor("red");
  selected = FALSE;
  saveFlag = FALSE;
  high = 0;
  low = 0;
  handleWidth = 6;
}

BaseCO::~BaseCO ()
{
  qDeleteAll(grabHandles);
  qDeleteAll(selectionArea);
}

void BaseCO::draw (QPixmap &, BarData *, int, int, int, Scaler &)
{
}

void BaseCO::getHighLow (double &h, double &l)
{
  h = high;
  l = low;
}

void BaseCO::getInfo (Setting &)
{
}

void BaseCO::dialog ()
{
}

void BaseCO::setDate (QDateTime &)
{
}

void BaseCO::getDate (QDateTime &)
{
}

void BaseCO::setDate2 (QDateTime &)
{
}

void BaseCO::getDate2 (QDateTime &)
{
}

void BaseCO::setPrice (double)
{
}

void BaseCO::setPrice2 (double)
{
}

void BaseCO::setSettings (Setting &)
{
}

void BaseCO::getSettings (QString &)
{
}

void BaseCO::create (QString &, QString &, QString &)
{
}

int BaseCO::create2 (QDateTime &, double)
{
  return 0;
}

int BaseCO::create3 (QDateTime &, double)
{
  return 0;
}

void BaseCO::moving (QDateTime &, double, int)
{
}







void BaseCO::setSaveFlag (int d)
{
  saveFlag = d;
}

int BaseCO::getSaveFlag ()
{
  return saveFlag;
}

void BaseCO::setSelected (int d)
{
  selected = d;
}

int BaseCO::getSelected ()
{
  return selected;
}

void BaseCO::clearGrabHandles ()
{
  qDeleteAll(grabHandles);
  grabHandles.clear();
}

void BaseCO::setGrabHandle (QRegion *d)
{
  grabHandles.append(d);
}

void BaseCO::clearSelectionArea ()
{
  qDeleteAll(selectionArea);
  selectionArea.clear();
}

void BaseCO::setSelectionArea (QRegion *d)
{
  selectionArea.append(d);
}

int BaseCO::isSelected (QPoint &point)
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

int BaseCO::isGrabSelected (QPoint &point)
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

void BaseCO::getCOList (QStringList &l)
{
  l.clear();
  l << QObject::tr("Buy Arrow") << QObject::tr("Horizontal Line") << QObject::tr("Retracement");
  l << QObject::tr("Sell Arrow") << QObject::tr("Text") << QObject::tr("Trend Line");
  l << QObject::tr("Vertical Line");
}

void BaseCO::setSymbol (QString &d)
{
  symbol = d;
}

void BaseCO::getSymbol (QString &d)
{
  d = symbol;
}

void BaseCO::setID (QString &d)
{
  id = d;
}

void BaseCO::getID (QString &d)
{
  d = id;
}

void BaseCO::setIndicator (QString &d)
{
  indicator = d;
}

void BaseCO::getIndicator (QString &d)
{
  d = indicator;
}

int BaseCO::getType ()
{
  return type;
}

void BaseCO::setHigh (double d)
{
  high = d;
}

double BaseCO::getHigh ()
{
  return high;
}

void BaseCO::setLow (double d)
{
  low = d;
}

double BaseCO::getLow ()
{
  return low;
}

