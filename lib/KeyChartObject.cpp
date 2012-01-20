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

#include "KeyChartObject.h"

#include <QtDebug>

KeyChartObject::KeyChartObject ()
{
  _list << "ID";
  _list << "DATE";
  _list << "DATE2";
  _list << "PRICE";
  _list << "PRICE2";
  _list << "COLOR";
  _list << "EXTEND";
  _list << "TEXT";
  _list << "FONT";
  _list << "HIGH";
  _list << "LOW";
  _list << "LEVEL1";
  _list << "LEVEL2";
  _list << "LEVEL3";
  _list << "LEVEL4";
  _list << "LEVEL5";
  _list << "LEVEL6";
  _list << "SYMBOL";
  _list << "CHART";
  _list << "TYPE";
  _list << "Z";
  _list << "PEN";
  _list << "RO";
}
