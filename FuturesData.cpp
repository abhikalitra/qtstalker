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

#include "FuturesData.h"
#include <qobject.h>
#include <qdatetime.h>

FuturesData::FuturesData ()
{
}

FuturesData::~FuturesData ()
{
}

QString FuturesData::getName ()
{
  return name;
}

QString FuturesData::getSymbol ()
{
  return symbol;
}

QString FuturesData::getExchange ()
{
  return exchange;
}

QString FuturesData::getLimit ()
{
  return limit;
}

QString FuturesData::getCC ()
{
  return cc;
}

float FuturesData::getRate ()
{
  return rate;
}

QStringList FuturesData::getSymbolList ()
{
  return QStringList::split(",", SYMBOLS, FALSE);
}

QStringList FuturesData::getMonthList ()
{
  return monthList;
}

int FuturesData::setSymbol (QString d)
{
  monthList.clear();

  QDate date = QDate::currentDate();
  ccFlag = FALSE;

  while (1)
  {
    if (! d.compare("AD"))
    {
      name = "Australian Dollar";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("BO"))
    {
      name = "Soybean Oil";
      symbol = d;
      rate = 600;
      monthList = QStringList::split(",", "F,H,K,N,Q,U,V,Z", FALSE);
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

    if (! d.compare("NB"))
    {
      name = "British Pound";
      symbol = d;
      rate = 625;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("C"))
    {
      name = "Corn";
      symbol = d;
      rate = 50;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("CO") || ! d.compare("CC"))
    {
      name = "Cocoa";
      symbol = "CC";
      rate = 10;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("CD"))
    {
      name = "Canadian Dollar";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("CL"))
    {
      name = "Crude Oil";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "F,G,H,J,K,M,N,Q,U,V,X,Z", FALSE);
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("CR"))
    {
      name = "CRB Index";
      symbol = d;
      monthList = QStringList::split(",", "F,G,J,M,Q,X", FALSE);
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
      name = "Cotton";
      symbol = d;
      rate = 500;
      monthList = QStringList::split(",", "H,K,N,V,Z", FALSE);
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
      name = "Dow Jones Industrial Average";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }


    if (! d.compare("DX"))
    {
      name = "US Dollar Index";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("EC"))
    {
      name = "Euro";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("ED"))
    {
      name = "Eurodollar";
      symbol = d;
      rate = 2500;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("ES"))
    {
      name = "E-MINI S&P 500";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("FC"))
    {
      name = "Feeder Cattle";
      symbol = d;
      rate = 500;
      monthList = QStringList::split(",", "F,H,J,K,Q,U,V,X", FALSE);
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
      name = "Gold";
      symbol = d;
      rate = 100;
      monthList = QStringList::split(",", "G,J,M,Q,V,Z", FALSE);
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
      name = "Goldman Sachs Commodity Index";
      symbol = d;
      monthList = QStringList::split(",", "F,G,H,J,K,M,N,Q,U,V,X,Z", FALSE);
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
      name = "Copper";
      symbol = d;
      rate = 250;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("HO"))
    {
      name = "Heating Oil";
      symbol = d;
      rate = 400;
      monthList = QStringList::split(",", "F,G,H,J,K,M,N,Q,U,V,X,Z", FALSE);
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("HU"))
    {
      name = "Unleaded Gasoline";
      symbol = d;
      rate = 400;
      monthList = QStringList::split(",", "F,G,H,J,K,M,N,Q,U,V,X,Z", FALSE);
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("OJ") || ! d.compare("JO"))
    {
      name = "Frozen Concentrated Orange Juice";
      symbol = "JO";
      rate = 150;
      monthList = QStringList::split(",", "F,H,K,N,U,X", FALSE);
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
      name = "Japanese Yen";
      symbol = d;
      rate = 1250;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("KC"))
    {
      name = "Coffee";
      symbol = d;
      rate = 375;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("LB"))
    {
      name = "Lumber";
      symbol = d;
      rate = 80;
      monthList = QStringList::split(",", "F,H,K,N,U,X", FALSE);
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
      name = "Live Cattle";
      symbol = d;
      rate = 400;
      monthList = QStringList::split(",", "G,J,M,Q,V,Z", FALSE);
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

    if (! d.compare("LN"))
    {
      name = "Lean Hogs";
      symbol = d;
      rate = 400;
      monthList = QStringList::split(",", "G,J,M,N,Q,V,Z", FALSE);
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
      name = "NASDAQ 100";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("NG"))
    {
      name = "Natural Gas";
      symbol = d;
      rate = 10000;
      monthList = QStringList::split(",", "F,G,H,J,K,M,N,Q,U,V,X,Z", FALSE);
      ccMonth3(date.month());
      break;
    }

    if (! d.compare("NQ"))
    {
      name = "E-MINI NASDAQ 100";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("O"))
    {
      name = "Oats";
      symbol = d;
      rate = 50;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    if (! d.compare("PA"))
    {
      name = "Palladium";
      symbol = d;
      rate = 100;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("PB"))
    {
      name = "Frozen Pork Bellies";
      symbol = d;
      rate = 400;
      monthList = QStringList::split(",", "G,H,K,N,Q", FALSE);
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
      name = "Platinum";
      symbol = d;
      rate = 50;
      monthList = QStringList::split(",", "F,J,N,V", FALSE);
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
      name = "Soybeans";
      symbol = d;
      rate = 50;
      monthList = QStringList::split(",", "F,H,K,N,Q,U,X", FALSE);
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
      name = "Sugar";
      symbol = d;
      rate = 1120;
      monthList = QStringList::split(",", "H,K,N,V", FALSE);
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
      name = "Swiss Franc";
      symbol = d;
      rate = 1250;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("SI"))
    {
      name = "Silver";
      symbol = d;
      rate = 100;
      monthList = QStringList::split(",", "F,H,K,N,U,Z", FALSE);
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
      name = "Soy Meal";
      symbol = d;
      rate = 100;
      monthList = QStringList::split(",", "F,H,K,N,Q,U,V,Z", FALSE);
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
      name = "S&P 500";
      symbol = d;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("TY"))
    {
      name = "Treasury Note 10 yr.";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("US"))
    {
      name = "US Treasury Bond";
      symbol = d;
      rate = 1000;
      monthList = QStringList::split(",", "H,M,U,Z", FALSE);
      ccMonth1(date.month());
      break;
    }

    if (! d.compare("W"))
    {
      name = "Wheat";
      symbol = d;
      rate = 50;
      monthList = QStringList::split(",", "H,K,N,U,Z", FALSE);
      ccMonth2(date.month());
      break;
    }

    return TRUE;
  }

  if (ccFlag)
    cc.prepend(QString::number(date.year() + 1));
  else
    cc.prepend(QString::number(date.year()));
  cc.prepend(symbol);

  return FALSE;
}

void FuturesData::ccMonth1 (int month)
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

void FuturesData::ccMonth2 (int month)
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

void FuturesData::ccMonth3 (int month)
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



