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

#include "COFactory.h"
#include "Buy.h"
#include "HLine.h"
#include "Retracement.h"
#include "Sell.h"
#include "Text.h"
#include "TLine.h"
#include "VLine.h"

#include <QDebug>

COFactory::COFactory ()
{
}

COPlugin * COFactory::getCO (QString &co)
{
  QStringList l;
  l << "Buy" << "HLine" << "Retracement" << "Sell" << "Text" << "TLine" << "VLine";
  
  COPlugin *plug = 0;
  
  switch ((COType) l.indexOf(co))
  {
    case COTypeBuy:
      plug = new Buy;
      break;
    case COTypeHLine:
      plug = new HLine;
      break;
    case COTypeRetracement:
      plug = new Retracement;
      break;
    case COTypeSell:
      plug = new Sell;
      break;
    case COTypeText:
      plug = new Text;
      break;
    case COTypeTLine:
      plug = new TLine;
      break;
    case COTypeVLine:
      plug = new VLine;
      break;
    default:
      break;
  }
  
  return plug;
}

