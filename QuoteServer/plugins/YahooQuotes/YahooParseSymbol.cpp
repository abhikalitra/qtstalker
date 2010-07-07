/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "YahooParseSymbol.h"

#include <QDebug>

YahooParseSymbol::YahooParseSymbol ()
{
  _exchangeList << "NYSE" << "AX" << "SA" << "TO" << "BO" << "NS" << "L" << "SI" << "B";
}

int YahooParseSymbol::parse (YahooUrlData &data)
{
  QString s;
  QStringList l = data.ysymbol.split(".");
  if (l.count() == 1)
    l.append("NYSE");

  int rc = 0;

  switch (_exchangeList.indexOf(l[1]))
  {
    // unfortunately yahoo does not separate NYSE,NASDAQ,ASE quotes
    // so we lump them into the NYSE exchange
    case 0: // NYSE
      data.symbol = l[0];
      data.exchange = "XNYS";
      break;
    case 1: // AX
      data.symbol = l[0];
      data.exchange = "XASX";
      break;
    case 2: // SA
      data.symbol = l[0];
      data.exchange = "BVMF";
      break;
    case 3: // TO
      data.symbol = l[0];
      data.exchange = "XTSE";
      break;
    case 4: // BO
    case 5: // NS
      data.symbol = l[0];
      data.exchange = "XNSE";
      break;
    case 6: // L
      data.symbol = l[0];
      data.exchange = "XLON";
      break;
    case 7: // SI
      data.symbol = l[0];
      data.exchange = "XSES";
      break;
    case 8: // NYSE .B shares ig BRK.B
      data.symbol = l[0] + ".B"; // we keep the .B
      data.exchange = "XNYS";
      break;
    default: // error
      rc = 1;
      break;
  }

  return rc;
}

