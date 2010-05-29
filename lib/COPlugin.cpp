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

#include "COPlugin.h"

#include <QDebug>

COPlugin::COPlugin ()
{
  _handleWidth = 6;
  _selected = 0;
  _saveFlag = 0;
  _id = 0;
}

// virtual
COPlugin::~COPlugin ()
{
  qDeleteAll(_grabHandles);
  qDeleteAll(_selectionArea);
}

// virtual
void COPlugin::draw (PlotData &, DateBar &, Scaler &)
{
}

// virtual
int COPlugin::getHighLow (double &, double &)
{
  return 0;
}

// virtual
void COPlugin::getInfo (Setting *)
{
}

// virtual
void COPlugin::dialog ()
{
}

// virtual
void COPlugin::load (QSqlQuery &)
{
}

// virtual
void COPlugin::save ()
{
}

// virtual
void COPlugin::create ()
{
}

// virtual
int COPlugin::create2 (QDateTime &, double)
{
  return 0;
}

// virtual
int COPlugin::create3 (QDateTime &, double)
{
  return 0;
}

// virtual
void COPlugin::moving (QDateTime &, double, int)
{
}

// virtual
void COPlugin::getIcon (QIcon &)
{
}

// virtual
int COPlugin::inDateRange (QDateTime &, QDateTime &, DateBar &)
{
  return 0;
}

// virtual
int COPlugin::CUS (QStringList &)
{
  return 0;
}

//************************************************
//************ NON VIRTUAL ***********************
//************************************************

void COPlugin::setSelected (int d)
{
  _selected = d;
}

void COPlugin::clearGrabHandles ()
{
  qDeleteAll(_grabHandles);
  _grabHandles.clear();
}

void COPlugin::setGrabHandle (QRegion *d)
{
  _grabHandles.append(d);
}

void COPlugin::clearSelectionArea ()
{
  qDeleteAll(_selectionArea);
  _selectionArea.clear();
}

void COPlugin::setSelectionArea (QRegion *d)
{
  _selectionArea.append(d);
}

int COPlugin::isSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) _selectionArea.count(); loop++)
  {
    QRegion *r = _selectionArea.at(loop);
    if (r->contains(point))
      return 1;
  }

  return 0;
}

int COPlugin::isGrabSelected (QPoint &point)
{
  int loop;
  for (loop = 0; loop < (int) _grabHandles.count(); loop++)
  {
    QRegion *r = _grabHandles.at(loop);
    if (r->contains(point))
      return loop + 1;
  }

  return 0;
}

int COPlugin::getID ()
{
  return _id;
}

void COPlugin::setID (int d)
{
  _id = d;
}

void COPlugin::setSymbol (QString &d)
{
  _symbol = d;
}

void COPlugin::setIndicator (QString &d)
{
  _indicator = d;
}

void COPlugin::setExchange (QString &d)
{
  _exchange = d;
}

