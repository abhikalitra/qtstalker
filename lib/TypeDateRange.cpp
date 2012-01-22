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

#include "TypeDateRange.h"

#include <QDebug>
#include <QObject>

TypeDateRange::TypeDateRange ()
{
  _list << QObject::tr("1 Day");
  _list << QObject::tr("1 Week");
  _list << QObject::tr("1 Month");
  _list << QObject::tr("3 Months");
  _list << QObject::tr("6 Months");
  _list << QObject::tr("1 Year");
  _list << QObject::tr("2 Years");
  _list << QObject::tr("5 Years");
  _list << QObject::tr("10 Years");
  _list << QObject::tr("25 Years");
  _list << QObject::tr("50 Years");
  _list << QObject::tr("All");
}
