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

#ifndef MA_FACTORY_HPP
#define MA_FACTORY_HPP

#include <QStringList>
#include <QColor>
#include <QString>

#include "PlotLine.h"

class MAFactory
{
  public:
    enum Method
    {
      EMA,
      DEMA,
      KAMA,
      SMA,
      TEMA,
      TRIMA,
      _Wilder,
      WMA
    };

    MAFactory ();
    PlotLine * ma (PlotLine *, int period, int method, int lineType, QColor &);
    QStringList & list ();
    int typeFromString (QString &);
    
  private:
    QStringList _maList;
};

#endif