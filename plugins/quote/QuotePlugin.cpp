/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "QuotePlugin.h"
#include <qdatetime.h>

QuotePlugin::QuotePlugin ()
{
  pluginType = "Quote";
}

QuotePlugin::~QuotePlugin ()
{
}

QString QuotePlugin::stripJunk (QString d)
{
  QString s = d.stripWhiteSpace();

  while (1)
  {
    int p = s.find('"', 0, TRUE);
    if (p == -1)
      break;
    else
      s.remove(p, 1);
  }

  return s;
}

bool QuotePlugin::setTFloat (QString d)
{
  QString s = d;
  int p = s.find("A", 0, TRUE);
  if (p != -1)
    s.remove(p, 1);

  p = s.find("B", 0, TRUE);
  if (p != -1)
    s.remove(p, 1);

  p = s.find("K", 0, TRUE);
  if (p != -1)
  {
    s.remove(p, 1);
    s.append("000");
  }

  while (s.contains(","))
  {
    int pos = s.find(",", 0, TRUE);
    s.remove(pos, 1);
  }

  bool ok;
  tfloat = s.toFloat(&ok);
  if (! ok)
    return TRUE;
  else
    return FALSE;
}

int QuotePlugin::setFutures (QString d)
{
  QDate date = QDate::currentDate();
  ccFlag = FALSE;

  while (1)
  {
    if (! d.compare("AD"))
    {
      futureName = "Australian Dollar";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("BO"))
    {
      futureName = "Soybean Oil";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        case 8:
          cc = 'U';
	  break;
        case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("BP") || ! d.compare("NB"))
    {
      futureName = "British Pound";
      futureSymbol = "NB";
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("C"))
    {
      futureName = "Corn";
      futureSymbol = d;
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("CO") || ! d.compare("CC"))
    {
      futureName = "Cocoa";
      futureSymbol = "CC";
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("CD"))
    {
      futureName = "Canadian Dollar";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("CL"))
    {
      futureName = "Crude Oil";
      futureSymbol = d;
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("CR"))
    {
      futureName = "CRB Index";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
	case 3:
          cc = 'J';
	  break;
        case 4:
	case 5:
          cc = 'M';
	  break;
        case 6:
	case 7:
	  cc = 'Q';
	  break;
	case 8:
	case 9:
	case 10:
          cc = 'X';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("CT"))
    {
      futureName = "Cotton";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
	case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'H';
	  break;
      }
      break;
    }

    if (! d.compare("DJ"))
    {
      futureName = "Dow Jones Industrial Average";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }


    if (! d.compare("DX"))
    {
      futureName = "US Dollar Index";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("EC"))
    {
      futureName = "Euro";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("ED"))
    {
      futureName = "Eurodollar";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("ES"))
    {
      futureName = "E-MINI S&P 500";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("FC"))
    {
      futureName = "Feeder Cattle";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
	  cc = 'J';
	  break;
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
	case 7:
          cc = 'Q';
	  break;
        case 8:
          cc = 'U';
	  break;
        case 9:
          cc = 'V';
	  break;
        case 10:
          cc = 'X';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("GC"))
    {
      futureName = "Gold";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
	case 3:
          cc = 'J';
	  break;
        case 4:
          cc = 'M';
	  break;
        case 5:
        case 6:
	case 7:
          cc = 'Q';
	  break;
        case 8:
	case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("GI"))
    {
      futureName = "Goldman Sachs Commodity Index";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
          cc = 'H';
	  break;
        case 3:
	  cc = 'J';
	  break;
        case 4:
          cc = 'K';
	  break;
        case 5:
	  cc = 'M';
	  break;
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        case 8:
          cc = 'U';
	  break;
        case 9:
          cc = 'V';
	  break;
        case 10:
	  cc = 'X';
	  break;
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }

      break;
    }

    if (! d.compare("HG"))
    {
      futureName = "Copper";
      futureSymbol = d;
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("HO"))
    {
      futureName = "Heating Oil";
      futureSymbol = d;
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("HU"))
    {
      futureName = "Unleaded Gasoline";
      futureSymbol = d;
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("OJ") || ! d.compare("JO"))
    {
      futureName = "Frozen Concentrated Orange Juice";
      futureSymbol = "JO";
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
          cc = 'U';
	  break;
        case 9:
	case 10:
          cc = 'X';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("JY"))
    {
      futureName = "Japanese Yen";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("KC"))
    {
      futureName = "Coffee";
      futureSymbol = d;
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("LB"))
    {
      futureName = "Lumber";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
          cc = 'U';
	  break;
        case 9:
        case 10:
	  cc = 'X';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("LC"))
    {
      futureName = "Live Cattle";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
        case 3:
	  cc = 'J';
	  break;
        case 4:
        case 5:
	  cc = 'M';
	  break;
        case 6:
        case 7:
          cc = 'Q';
	  break;
        case 8:
        case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'G';
	  break;
      }
      break;
    }

    if (! d.compare("LH") || ! d.compare("LN"))
    {
      futureName = "Lean Hogs";
      futureSymbol = "LN";
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
        case 3:
	  cc = 'J';
	  break;
        case 4:
        case 5:
	  cc = 'M';
	  break;
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        case 8:
        case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'G';
	  break;
      }
      break;
    }

    if (! d.compare("ND"))
    {
      futureName = "NASDAQ 100";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("NG"))
    {
      futureName = "Natural Gas";
      futureSymbol = d;
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("NQ"))
    {
      futureName = "E-MINI NASDAQ 100";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("O"))
    {
      futureName = "Oats";
      futureSymbol = d;
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("PA"))
    {
      futureName = "Palladium";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("PB"))
    {
      futureName = "Frozen Pork Bellies";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
	  cc = 'G';
	  break;
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'G';
	  break;
      }
      break;
    }

    if (! d.compare("PL"))
    {
      futureName = "Platinum";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
	case 3:
          cc = 'J';
	  break;
        case 4:
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
        case 9:
          cc = 'V';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("S"))
    {
      futureName = "Soybeans";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        case 8:
          cc = 'U';
	  break;
        case 9:
        case 10:
	  cc = 'X';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("SB"))
    {
      futureName = "Sugar";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
	case 9:
          cc = 'V';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'H';
	  break;
      }
      break;
    }

    if (! d.compare("SF"))
    {
      futureName = "Swiss Franc";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("SI"))
    {
      futureName = "Silver";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
        case 8:
          cc = 'U';
	  break;
        case 9:
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("SM"))
    {
      futureName = "Soy Meal";
      futureSymbol = d;
      switch (date.month())
      {
        case 1:
        case 2:
          cc = 'H';
	  break;
        case 3:
        case 4:
          cc = 'K';
	  break;
        case 5:
        case 6:
          cc = 'N';
	  break;
        case 7:
          cc = 'Q';
	  break;
        case 8:
          cc = 'U';
	  break;
        case 9:
          cc = 'V';
	  break;
        case 10:
        case 11:
          cc = 'Z';
	  break;
        default:
	  ccFlag = TRUE;
          cc = 'F';
	  break;
      }
      break;
    }

    if (! d.compare("SP"))
    {
      futureName = "S&P 500";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("TY"))
    {
      futureName = "Treasury Note 10 yr.";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("US"))
    {
      futureName = "US Treasury Bond";
      futureSymbol = d;
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("W"))
    {
      futureName = "Wheat";
      futureSymbol = d;
      ccMonth2(date.month());
      break;
    }

    return TRUE;
  }

  if (ccFlag)
    cc.prepend(QString::number(date.year() + 1));
  else
    cc.prepend(QString::number(date.year()));
  cc.prepend(futureSymbol);

  return FALSE;
}

void QuotePlugin::ccMonth1 (int month)
{
  switch (month)
  {
    case 1:
    case 2:
      cc = 'H';
      break;
    case 3:
    case 4:
    case 5:
      cc = 'M';
      break;
    case 6:
    case 7:
    case 8:
      cc = 'U';
      break;
    case 9:
    case 10:
    case 11:
      cc = 'Z';
      break;
    default:
      ccFlag = TRUE;
      cc = 'H';
      break;
  }
}

void QuotePlugin::ccMonth2 (int month)
{
  switch (month)
  {
    case 1:
    case 2:
      cc = 'H';
      break;
    case 3:
    case 4:
      cc = 'K';
      break;
    case 5:
    case 6:
      cc = 'N';
      break;
    case 7:
    case 8:
      cc = 'U';
      break;
    case 9:
    case 10:
    case 11:
      cc = 'Z';
      break;
    default:
      ccFlag = TRUE;
      cc = 'H';
      break;
  }
}

void QuotePlugin::ccMonth3 (int month)
{
  switch (month)
  {
    case 1:
      cc = 'H';
      break;
    case 2:
      cc = 'J';
      break;
    case 3:
      cc = 'K';
      break;
    case 4:
      cc = 'M';
      break;
    case 5:
      cc = 'N';
      break;
    case 6:
      cc = 'Q';
      break;
    case 7:
      cc = 'U';
      break;
    case 8:
      cc = 'V';
      break;
    case 9:
      cc = 'X';
      break;
    case 10:
      cc = 'Z';
      break;
    case 11:
      ccFlag = TRUE;
      cc = 'F';
      break;
    default:
      ccFlag = TRUE;
      cc = 'G';
      break;
  }
}

void QuotePlugin::setDataPath (QString d)
{
  dataPath = d;
}

