/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

FuturesData::FuturesData ()
{
  limit = 0;
  data.setAutoDelete(TRUE);
  loadData();
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

QString FuturesData::getContract ()
{
  return contract;
}

float FuturesData::getLimit ()
{
  return limit;
}

float FuturesData::getRate ()
{
  return rate;
}

void FuturesData::getSymbolList (QString &d, QStringList &l)
{
  l.clear();
  QDictIterator<Setting> it(data);
  for (; it.current(); ++it)
  {
    if (! d.compare("All"))
      l.append(it.current()->getData("Symbol"));
    else
    {
      if (! it.current()->getData("Exchange").compare(d))
        l.append(it.current()->getData("Symbol"));
    }
  }
}

void FuturesData::getMonthList (QStringList &l)
{
  l.clear();
  l = monthList;
}

void FuturesData::getMonths (QStringList &l)
{
  l.clear();
  l.append("F");
  l.append("G");
  l.append("H");
  l.append("J");
  l.append("K");
  l.append("M");
  l.append("N");
  l.append("Q");
  l.append("U");
  l.append("V");
  l.append("X");
  l.append("Z");
}

int FuturesData::setSymbol (QString &d)
{
  monthList.clear();
  
  Setting *set = data[d];
  if (! set)
    return TRUE;
    
  name = set->getData("Name");
  symbol = set->getData("Symbol");
  rate = set->getFloat("Rate");
  monthList = QStringList::split(",", set->getData("Month"), FALSE);
  limit = set->getFloat("Limit");
  exchange = set->getData("Exchange");
  QDateTime dt = QDateTime::currentDateTime();
  contract = getCurrentContract(dt);
  
  return FALSE;
}

void FuturesData::loadData ()
{
  data.clear();
  
  Setting *set = new Setting;
  set->setData("Name", QObject::tr("Australian Dollar"));
  set->setData("Symbol", "AD");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("AD", set);
  
  set = new Setting;
  set->setData("Name", QObject::tr("Soybean Oil"));
  set->setData("Symbol", "BO");
  set->setData("Rate", "600");
  set->setData("Month", "F,H,K,N,Q,U,V,Z");
  set->setData("Limit", "2");
  set->setData("Exchange", "CBOT");
  data.replace("BO", set);
  
  set = new Setting;
  set->setData("Name", QObject::tr("British Pound"));
  set->setData("Symbol", "NB");
  set->setData("Rate", "625");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("NB", set);

  set = new Setting;
  set->setData("Name", QObject::tr("British Pound"));
  set->setData("Symbol", "NB");
  set->setData("Rate", "625");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("BP", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Corn"));
  set->setData("Symbol", "C");
  set->setData("Rate", "50");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Limit", "20");
  set->setData("Exchange", "CBOT");
  data.replace("C", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Cocoa"));
  set->setData("Symbol", "CC");
  set->setData("Rate", "10");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Exchange", "NYBOT");
  data.replace("CO", set);
  
  set = new Setting;
  set->setData("Name", QObject::tr("Cocoa"));
  set->setData("Symbol", "CC");
  set->setData("Rate", "10");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Exchange", "NYBOT");
  data.replace("CC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Canadian Dollar"));
  set->setData("Symbol", "CD");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("CD", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Crude Oil"));
  set->setData("Symbol", "CL");
  set->setData("Rate", "1000");
  set->setData("Month", "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData("Limit", "3");
  set->setData("Exchange", "NYMEX");
  data.replace("CL", set);

  set = new Setting;
  set->setData("Name", QObject::tr("CRB Index"));
  set->setData("Symbol", "CR");
  set->setData("Month", "F,G,J,M,Q,X");
  set->setData("Exchange", "NYBOT");
  data.replace("CR", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Cotton"));
  set->setData("Symbol", "CT");
  set->setData("Rate", "500");
  set->setData("Month", "H,K,N,V,Z");
  set->setData("Limit", "3");
  set->setData("Exchange", "NYBOT");
  data.replace("CT", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Dow Jones Industrial Average"));
  set->setData("Symbol", "DJ");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CBOT");
  data.replace("DJ", set);
  
  set = new Setting;
  set->setData("Name", QObject::tr("US Dollar Index"));
  set->setData("Symbol", "DX");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "NYBOT");
  data.replace("DX", set);
  
  set = new Setting;
  set->setData("Name", QObject::tr("Euro"));
  set->setData("Symbol", "EC");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("EC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Eurodollar"));
  set->setData("Symbol", "ED");
  set->setData("Rate", "2500");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("ED", set);

  set = new Setting;
  set->setData("Name", QObject::tr("E-MINI S&P 500"));
  set->setData("Symbol", "ES");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("ES", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Feeder Cattle"));
  set->setData("Symbol", "FC");
  set->setData("Rate", "500");
  set->setData("Month", "F,H,J,K,Q,U,V,X");
  set->setData("Limit", "1.5");
  set->setData("Exchange", "CME");
  data.replace("FC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Gold"));
  set->setData("Symbol", "GC");
  set->setData("Rate", "100");
  set->setData("Month", "G,J,M,Q,V,Z");
  set->setData("Limit", "75");
  set->setData("Exchange", "NYMEX");
  data.replace("GC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Goldman Sachs Commodity Index"));
  set->setData("Symbol", "GI");
  set->setData("Month", "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData("Exchange", "CME");
  data.replace("GI", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Copper"));
  set->setData("Symbol", "HG");
  set->setData("Rate", "250");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Limit", "20");
  set->setData("Exchange", "NYMEX");
  data.replace("HG", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Heating Oil"));
  set->setData("Symbol", "HO");
  set->setData("Rate", "400");
  set->setData("Month", "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData("Limit", "6");
  set->setData("Exchange", "NYMEX");
  data.replace("HO", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Unleaded Gasoline"));
  set->setData("Symbol", "HU");
  set->setData("Rate", "400");
  set->setData("Month", "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData("Limit", "6");
  set->setData("Exchange", "NYMEX");
  data.replace("HU", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Frozen Concentrated Orange Juice"));
  set->setData("Symbol", "OJ");
  set->setData("Rate", "150");
  set->setData("Month", "F,H,K,N,U,X");
  set->setData("Limit", "5");
  set->setData("Exchange", "NYBOT");
  data.replace("OJ", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Frozen Concentrated Orange Juice"));
  set->setData("Symbol", "OJ");
  set->setData("Rate", "150");
  set->setData("Month", "F,H,K,N,U,X");
  set->setData("Limit", "5");
  set->setData("Exchange", "NYBOT");
  data.replace("JO", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Japanese Yen"));
  set->setData("Symbol", "JY");
  set->setData("Rate", "1250");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("JY", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Coffee"));
  set->setData("Symbol", "KC");
  set->setData("Rate", "375");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Exchange", "NYBOT");
  data.replace("KC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Lumber"));
  set->setData("Symbol", "LB");
  set->setData("Rate", "80");
  set->setData("Month", "F,H,K,N,U,X");
  set->setData("Limit", "10");
  set->setData("Exchange", "CME");
  data.replace("LB", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Live Cattle"));
  set->setData("Symbol", "LC");
  set->setData("Rate", "400");
  set->setData("Month", "G,J,M,Q,V,Z");
  set->setData("Limit", "1.5");
  set->setData("Exchange", "CME");
  data.replace("LC", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Lean Hogs"));
  set->setData("Symbol", "LN");
  set->setData("Rate", "400");
  set->setData("Month", "G,J,M,N,Q,V,Z");
  set->setData("Limit", "2");
  set->setData("Exchange", "CME");
  data.replace("LN", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Lean Hogs"));
  set->setData("Symbol", "LN");
  set->setData("Rate", "400");
  set->setData("Month", "G,J,M,N,Q,V,Z");
  set->setData("Limit", "2");
  set->setData("Exchange", "CME");
  data.replace("LH", set);
    
  set = new Setting;
  set->setData("Name", QObject::tr("NASDAQ 100"));
  set->setData("Symbol", "ND");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("ND", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Natural Gas"));
  set->setData("Symbol", "NG");
  set->setData("Rate", "10000");
  set->setData("Month", "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData("Limit", "1");
  set->setData("Exchange", "NYMEX");
  data.replace("NG", set);

  set = new Setting;
  set->setData("Name", QObject::tr("E-MINI NASDAQ 100"));
  set->setData("Symbol", "NQ");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("NQ", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Oats"));
  set->setData("Symbol", "O");
  set->setData("Rate", "50");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Limit", "20");
  set->setData("Exchange", "CBOT");
  data.replace("O", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Palladium"));
  set->setData("Symbol", "PA");
  set->setData("Rate", "100");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "NYMEX");
  data.replace("PA", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Frozen Pork Bellies"));
  set->setData("Symbol", "PB");
  set->setData("Rate", "400");
  set->setData("Month", "G,H,K,N,Q");
  set->setData("Limit", "3");
  set->setData("Exchange", "CME");
  data.replace("PB", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Platinum"));
  set->setData("Symbol", "PL");
  set->setData("Rate", "50");
  set->setData("Month", "F,J,N,V");
  set->setData("Limit", "50");
  set->setData("Exchange", "NYMEX");
  data.replace("PL", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Soybeans"));
  set->setData("Symbol", "S");
  set->setData("Rate", "50");
  set->setData("Month", "F,H,K,N,Q,U,X");
  set->setData("Limit", "50");
  set->setData("Exchange", "CBOT");
  data.replace("S", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Sugar #11 World"));
  set->setData("Symbol", "SB");
  set->setData("Rate", "1120");
  set->setData("Month", "H,K,N,V");
  set->setData("Exchange", "NYBOT");
  data.replace("SB", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Swiss Franc"));
  set->setData("Symbol", "SF");
  set->setData("Rate", "1250");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("SF", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Silver"));
  set->setData("Symbol", "SI");
  set->setData("Rate", "100");
  set->setData("Month", "F,H,K,N,U,Z");
  set->setData("Limit", "150");
  set->setData("Exchange", "NYMEX");
  data.replace("SI", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Soy Meal"));
  set->setData("Symbol", "SM");
  set->setData("Rate", "100");
  set->setData("Month", "F,H,K,N,Q,U,V,Z");
  set->setData("Limit", "20");
  set->setData("Exchange", "CBOT");
  data.replace("SM", set);

  set = new Setting;
  set->setData("Name", QObject::tr("S&P 500"));
  set->setData("Symbol", "SP");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CME");
  data.replace("SP", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Treasury Note 10 yr."));
  set->setData("Symbol", "TY");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CBOT");
  data.replace("TY", set);

  set = new Setting;
  set->setData("Name", QObject::tr("Treasury Note 10 yr."));
  set->setData("Symbol", "TY");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CBOT");
  data.replace("TYD", set);
    
  set = new Setting;
  set->setData("Name", QObject::tr("US Treasury Bond"));
  set->setData("Symbol", "US");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CBOT");
  data.replace("US", set);

  set = new Setting;
  set->setData("Name", QObject::tr("US Treasury Bond"));
  set->setData("Symbol", "US");
  set->setData("Rate", "1000");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "CBOT");
  data.replace("USD", set);
    
  set = new Setting;
  set->setData("Name", QObject::tr("Wheat"));
  set->setData("Symbol", "W");
  set->setData("Rate", "50");
  set->setData("Month", "H,K,N,U,Z");
  set->setData("Limit", "30");
  set->setData("Exchange", "CBOT");
  data.replace("W", set);

  set = new Setting;
  set->setData("Name", QObject::tr("NYSE"));
  set->setData("Symbol", "YX");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "NYBOT");
  data.replace("YX", set);
  
  // Warsaw Stock Exchange futures
  set = new Setting;
  set->setData("Name", QObject::tr("WIG20 Index"));
  set->setData("Symbol", "FW20");
  set->setData("Month", "H,M,U,Z");
  set->setData("Exchange", "WSE");
  set->setData("Rate", "10");
  data.replace("FW20", set);
}

QString FuturesData::getCurrentContract (QDateTime &dt)
{
  QString contract = symbol;
  bool yearFlag = FALSE;

  QStringList ml;
  getMonthList(ml);
  QStringList fml;
  getMonths(fml);

  int currentMonth = dt.date().month() - 1;

  int i = ml.findIndex(fml[currentMonth]);
  if (i != -1)
  {
    currentMonth++;

    if (currentMonth == 12)
    {
      yearFlag = TRUE;
      currentMonth = 0;
    }
  }

  if (! symbol.compare("CL") ||
      ! symbol.compare("HO") ||
      ! symbol.compare("HU") ||
      ! symbol.compare("NG"))
  {
    currentMonth++;

    if (currentMonth == 12)
    {
      yearFlag = TRUE;
      currentMonth = 0;
    }
  }

  while (1)
  {
    QString s = fml[currentMonth];
    int i = ml.findIndex(s);
    if (i != -1)
    {
      if (yearFlag)
        contract.append(QString::number(dt.date().year() + 1));
      else
        contract.append(QString::number(dt.date().year()));

      contract.append(fml[currentMonth]);

      break;
    }
    else
    {
      currentMonth++;
      
      if (currentMonth == 12)
      {
        yearFlag = TRUE;
        currentMonth = 0;
      }
    }
  }

  return contract;
}

