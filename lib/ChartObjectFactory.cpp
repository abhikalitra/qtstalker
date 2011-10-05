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
#include "ChartObjectType.h"
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
}

ChartObject * ChartObjectFactory::chartObject (QString type)
{
  ChartObject *co = 0;
  ChartObjectType cot;

  switch ((ChartObjectType::Type) cot.stringToType(type))
  {
    case ChartObjectType::_BUY:
      co = new ChartObjectBuy;
      break;
    case ChartObjectType::_HLINE:
      co = new ChartObjectHLine;
      break;
    case ChartObjectType::_RETRACEMENT:
      co = new ChartObjectRetracement;
      break;
    case ChartObjectType::_SELL:
      co = new ChartObjectSell;
      break;
    case ChartObjectType::_TEXT:
      co = new ChartObjectText;
      break;
    case ChartObjectType::_TLINE:
      co = new ChartObjectTLine;
      break;
    case ChartObjectType::_VLINE:
      co = new ChartObjectVLine;
      break;
    default:
      break;
  }

  return co;
}
