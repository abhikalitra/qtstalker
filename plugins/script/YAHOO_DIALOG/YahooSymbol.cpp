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

#include "YahooSymbol.h"

#include <QtDebug>

YahooSymbol::YahooSymbol ()
{
  _yexchange << "NYSE" << "AX" << "SA" << "TO" << "BO" << "NS" << "L" << "B" << "PA";
}

int YahooSymbol::data (Setting &symbol)
{
  QStringList l = symbol.data("SYMBOL").split(".");
  if (l.count() == 1)
    l.append("NYSE");

  int rc = 0;
  symbol.setData("EXCHANGE", QString("YAHOO"));
  symbol.setData("TIME", QString("160000"));

  switch (_yexchange.indexOf(l.at(1)))
  {
    // unfortunately yahoo does not separate NYSE,NASDAQ,ASE quotes
    // so we lump them into the NYSE exchange
    case 0: // NYSE
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XNYS");
      symbol.setData("TIME", QString("160000"));
      break;
    case 1: // AX
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XASX");
      symbol.setData("TIME", QString("160000"));
      break;
    case 2: // SA
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "BVMF");
      symbol.setData("TIME", QString("170000"));
      break;
    case 3: // TO
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XTSE");
      symbol.setData("TIME", QString("160000"));
      break;
    case 4: // BO
    case 5: // NS
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XNSE");
      symbol.setData("TIME", QString("160000"));
      break;
    case 6: // L
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XLON");
      symbol.setData("TIME", QString("163500"));
      break;
    case 7: // B, some XNYS symbols use .B eg BRK.B
//      symbol.setData("SYMBOL", symbol.data("YSYMBOL"));
//      symbol.setData("EXCHANGE", "XNYS");
      symbol.setData("TIME", QString("160000"));
      break;
    case 8: // PA
//      symbol.setData("SYMBOL", l.at(0));
//      symbol.setData("EXCHANGE", "XPAR");
      symbol.setData("TIME", QString("173000"));
      break;
    default: // error
      rc = 1;
      break;
  }

  return rc;
}
