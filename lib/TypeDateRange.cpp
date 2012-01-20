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

TypeDateRange::TypeDateRange ()
{
  _list << "1 Day";
  _list << "1 Week";
  _list << "1 Month";
  _list << "3 Months";
  _list << "6 Months";
  _list << "1 Year";
  _list << "2 Years";
  _list << "5 Years";
  _list << "10 Years";
  _list << "25 Years";
  _list << "50 Years";
  _list << "All";
}
