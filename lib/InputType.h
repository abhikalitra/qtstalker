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

#ifndef INPUT_TYPE_HPP
#define INPUT_TYPE_HPP

#include <QStringList>
#include <QList>

#include "BarData.h"
#include "Curve.h"
#include "Script.h"
#include "ta_libc.h"

class InputType
{
  public:
    enum Type
    {
      _DATE,
      _OPEN,
      _HIGH,
      _LOW,
      _CLOSE,
      _VOLUME,
      _OI
    };

    InputType ();
    QStringList & list ();
    InputType::Type fromString (QString);
    Curve * input (BarData *, QString);
    Curve * ohlc (BarData *);
    int inputs (QList<Curve *> &, QStringList &, Script *, BarData *);
    int keys (QList<Curve *> &, QList<int> &);
    int fill (QList<Curve *> &, QList<int> &, TA_Real out[], TA_Real out2[], TA_Real out3[], TA_Real out4[]);
    int outputs (QList<Curve *> &, QList<int> &, int outNb, TA_Real out[], TA_Real out2[], TA_Real out3[]);

  private:
    QStringList _list;
};

#endif
