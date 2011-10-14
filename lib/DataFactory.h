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

#ifndef DATA_FACTORY_HPP
#define DATA_FACTORY_HPP

#include <QStringList>

#include "Data.h"

class DataFactory
{
  public:
    enum Type
    {
      _BAR_LENGTH,
      _BOOL,
      _CHART_OBJECT,
      _COLOR,
      _CURVE,
      _CURVE_BAR,
      _DATE_RANGE,
      _DATETIME,
      _DOUBLE,
      _FILE,
      _FONT,
      _INTEGER,
      _LIST,
      _MA,
      _OP,
      _STRING,
      _SYMBOL
    };

    DataFactory ();
    Data * data (QString);
    Data * data (int);
    int stringToType (QString);

  private:
    QStringList _types;
};

#endif
