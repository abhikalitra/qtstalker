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

#include "COPlugin.h"

#include <QDebug>

COPlugin::COPlugin ()
{
  handleWidth = 6;
}

// virtual
COPlugin::~COPlugin ()
{
}

// virtual
void COPlugin::draw (ChartObject *, QPixmap &, DateBar &, int, int, int, Scaler &)
{
}

// virtual
void COPlugin::getHighLow (ChartObject *)
{
}

// virtual
void COPlugin::getInfo (ChartObject *, Setting &)
{
}

// virtual
void COPlugin::dialog (ChartObject *)
{
}

// virtual
void COPlugin::setSettings (ChartObject *, QSqlQuery &)
{
}

// virtual
void COPlugin::getSettings (ChartObject *, QString &)
{
}

// virtual
void COPlugin::create (ChartObject *)
{
}

// virtual
int COPlugin::create2 (ChartObject *, QDateTime &, double)
{
  return 0;
}

// virtual
int COPlugin::create3 (ChartObject *, QDateTime &, double)
{
  return 0;
}

// virtual
void COPlugin::moving (ChartObject *, QDateTime &, double, int)
{
}

// virtual
void COPlugin::getIcon (QIcon &)
{
}

// virtual
int COPlugin::inDateRange (ChartObject *, QDateTime &, QDateTime &)
{
  return 0;
}

