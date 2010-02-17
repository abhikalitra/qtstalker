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

#include "COFactory.h"

#include "BuyArrow.h"
#include "HorizontalLine.h"
#include "Retracement.h"
#include "SellArrow.h"
#include "Text.h"
#include "TrendLine.h"
#include "VerticalLine.h"

#include <QStringList>

COFactory::COFactory ()
{
}

BaseCO * COFactory::getCO (QString &type)
{
  BaseCO bco;
  QStringList l;
  bco.getCOList(l);

  int i = l.indexOf(type);
  return getCO(i);
}

BaseCO * COFactory::getCO (int type)
{
  BaseCO *co = 0;

  switch ((BaseCO::COType) type)
  {
    case BaseCO::COBuyArrow:
    {
      co = new BuyArrow;
      break;
    }
    case BaseCO::COHorizontalLine:
    {
      co = new HorizontalLine;
      break;
    }
    case BaseCO::CORetracement:
    {
      co = new Retracement;
      break;
    }
    case BaseCO::COSellArrow:
    {
      co = new SellArrow;
      break;
    }
    case BaseCO::COText:
    {
      co = new Text;
      break;
    }
    case BaseCO::COTrendLine:
    {
      co = new TrendLine;
      break;
    }
    case BaseCO::COVerticalLine:
    {
      co = new VerticalLine;
      break;
    }
    default:
      break;
  }

  return co;
}

