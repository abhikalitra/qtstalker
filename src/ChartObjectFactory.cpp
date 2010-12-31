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

#include "ChartObjectFactory.h"
#include "ChartObjectBuy.h"
#include "ChartObjectHLine.h"
#include "ChartObjectRetracement.h"
#include "ChartObjectSell.h"
#include "ChartObjectText.h"
#include "ChartObjectTLine.h"
#include "ChartObjectVLine.h"

#include <QDebug>

ChartObjectFactory::ChartObjectFactory ()
{
  _types << "Buy" << "HLine" << "Retracement" << "Sell" << "Text" << "TLine" << "VLine";
}

ChartObject * ChartObjectFactory::chartObject (QString type)
{
  ChartObject *co = 0;
  
  switch ((Type) _types.indexOf(type))
  {
    case _BUY:
      co = new ChartObjectBuy;
      break;
    case _HLINE:
      co = new ChartObjectHLine;
      break;
    case _RETRACEMENT:
      co = new ChartObjectRetracement;
      break;
    case _SELL:
      co = new ChartObjectSell;
      break;
    case _TEXT:
      co = new ChartObjectText;
      break;
    case _TLINE:
      co = new ChartObjectTLine;
      break;
    case _VLINE:
      co = new ChartObjectVLine;
      break;
    default:
      break;
  }
  
  return co;
}
