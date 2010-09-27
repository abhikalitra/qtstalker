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

#ifndef FUNCTION_ADX_HPP
#define FUNCTION_ADX_HPP

#include <QStringList>

#include "Indicator.h"
#include "Curve.h"
#include "BarData.h"

class FunctionADX
{
  public:
    enum Method
    {
      _ADX,
      _ADXR,
      _PDI,
      _MDI,
      _DX
    };

    FunctionADX ();
    int script (QStringList &, Indicator &, BarData &);
    Curve * calculate (int period, int method, BarData &);

  protected:
    QStringList _methodList;
};

#endif
