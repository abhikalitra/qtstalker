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

#ifndef MA_TYPE_HPP
#define MA_TYPE_HPP

#include <QStringList>

#include "Data.h"

class MAType
{
  public:
    enum Type
    {
      _SMA,
      _EMA,
      _WMA,
      _DEMA,
      _TEMA,
      _TRIMA,
      _KAMA,
      _WILDER
    };

    MAType ();
    QStringList & list ();
    MAType::Type fromString (QString);
    Data * getMA (Data *, int period, int method);
    Data * getWilder (Data *, int period);

  private:
    QStringList _list;
};

#endif
