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

#ifndef FUNCTION_AROON_HPP
#define FUNCTION_AROON_HPP

#include <QStringList>
#include <QList>

#include "Indicator.h"
#include "Curve.h"
#include "BarData.h"

class FunctionAROON
{
  public:
    enum Method
    {
      _AROON,
      _AROONOSC
    };

    FunctionAROON ();
    int script (QStringList &, Indicator &, BarData &);
    int scriptAROON (QStringList &, Indicator &, BarData &);
    int scriptAROONOSC (QStringList &, Indicator &, BarData &);
    int getAROON (int period, QList<Curve *> &, BarData &);
    Curve * getAROONOSC (int period, BarData &);
    QStringList & list ();

  protected:
    QStringList _methodList;
};

#endif
