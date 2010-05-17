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

#include "MAFactory.h"
#include "EMA.h"
#include "DEMA.h"
#include "KAMA.h"
#include "SMA.h"
#include "TEMA.h"
#include "TRIMA.h"
#include "Wilder.h"
#include "WMA.h"

#include <QtDebug>

MAFactory::MAFactory ()
{
  _maList << "EMA" << "DEMA" << "KAMA" << "SMA" << "TEMA" << "TRIMA" << "Wilder" << "WMA";
}

PlotLine * MAFactory::ma (PlotLine *in, int period, int method, int lineType, QColor &color)
{
  PlotLine *line = 0;
  
  switch ((Method) method)
  {
    case _EMA:
    {
      EMA ma;
      line = ma.ema(in, period, lineType, color);
      break;
    }      
    case _DEMA:
    {
      DEMA ma;
      line = ma.dema(in, period, lineType, color);
      break;
    }
    case _KAMA:
    {
      KAMA ma;
      line = ma.kama(in, period, lineType, color);
      break;
    }
    case _SMA:
    {
      SMA ma;
      line = ma.sma(in, period, lineType, color);
      break;
    }
    case _TEMA:
    {
      TEMA ma;
      line = ma.tema(in, period, lineType, color);
      break;
    }
    case _TRIMA:
    {
      TRIMA ma;
      line = ma.trima(in, period, lineType, color);
      break;
    }
    case _Wilder:
    {
      Wilder ma;
      line = ma.wilder(in, period, lineType, color);
      break;
    }
    case _WMA:
    {
      WMA ma;
      line = ma.wma(in, period, lineType, color);
      break;
    }
    default:
      break;
  }

  return line;
}

void MAFactory::list (QStringList &l)
{
  l = _maList;
}

int MAFactory::typeFromString (QString &d)
{
  return _maList.indexOf(d);
}

