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
  
  nameKey = "Name";
  symbolKey = "Symbol";
  rateKey = "Rate";
  monthKey = "Month";
  exchangeKey = "Exchange";
  limitKey = "Limit";
  cme = "CME";
  cbot = "CBOT";
  nymex = "NYMEX";
  nybot = "NYBOT";
  
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
    {
      QString s = it.current()->getData(symbolKey);
      if (l.findIndex(s) == -1)
        l.append(s);
    }
    else
    {
      if (! it.current()->getData(exchangeKey).compare(d))
      {
        QString s = it.current()->getData(symbolKey);
        if (l.findIndex(s) == -1)
          l.append(s);
      }
    }
  }
  l.sort();
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
    
  name = set->getData(nameKey);
  symbol = set->getData(symbolKey);
  rate = set->getFloat(rateKey);
  monthList = QStringList::split(",", set->getData(monthKey), FALSE);
  limit = set->getFloat(limitKey);
  exchange = set->getData(exchangeKey);
  QDateTime dt = QDateTime::currentDateTime();
  contract = getCurrentContract(dt);
  
  return FALSE;
}

void FuturesData::loadData ()
{
  data.clear();
  
  Setting *set = new Setting;
  set->setData(nameKey, QObject::tr("Australian Dollar"));
  set->setData(symbolKey, "AD");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("AD", set);
  
  set = new Setting;
  set->setData(nameKey, QObject::tr("Soybean Oil"));
  set->setData(symbolKey, "BO");
  set->setData(rateKey, "600");
  set->setData(monthKey, "F,H,K,N,Q,U,V,Z");
  set->setData(limitKey, "2");
  set->setData(exchangeKey, cbot);
  data.replace("BO", set);
  
  set = new Setting;
  set->setData(nameKey, QObject::tr("British Pound"));
  set->setData(symbolKey, "NB");
  set->setData(rateKey, "625");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("NB", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("British Pound"));
  set->setData(symbolKey, "NB");
  set->setData(rateKey, "625");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("BP", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Corn"));
  set->setData(symbolKey, "C");
  set->setData(rateKey, "50");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(limitKey, "20");
  set->setData(exchangeKey, cbot);
  data.replace("C", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Cocoa"));
  set->setData(symbolKey, "CC");
  set->setData(rateKey, "10");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(exchangeKey, nybot);
  data.replace("CO", set);
  
  set = new Setting;
  set->setData(nameKey, QObject::tr("Cocoa"));
  set->setData(symbolKey, "CC");
  set->setData(rateKey, "10");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(exchangeKey, nybot);
  data.replace("CC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Canadian Dollar"));
  set->setData(symbolKey, "CD");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("CD", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Crude Oil"));
  set->setData(symbolKey, "CL");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData(limitKey, "3");
  set->setData(exchangeKey, nymex);
  data.replace("CL", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("CRB Index"));
  set->setData(symbolKey, "CR");
  set->setData(monthKey, "F,G,J,M,Q,X");
  set->setData(exchangeKey, nybot);
  data.replace("CR", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Cotton"));
  set->setData(symbolKey, "CT");
  set->setData(rateKey, "500");
  set->setData(monthKey, "H,K,N,V,Z");
  set->setData(limitKey, "3");
  set->setData(exchangeKey, nybot);
  data.replace("CT", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Dow Jones Industrial Average"));
  set->setData(symbolKey, "DJ");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cbot);
  data.replace("DJ", set);
  
  set = new Setting;
  set->setData(nameKey, QObject::tr("US Dollar Index"));
  set->setData(symbolKey, "DX");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, nybot);
  data.replace("DX", set);
  
  set = new Setting;
  set->setData(nameKey, QObject::tr("Euro"));
  set->setData(symbolKey, "EC");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("EC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Eurodollar"));
  set->setData(symbolKey, "ED");
  set->setData(rateKey, "2500");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("ED", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("E-MINI S&P 500"));
  set->setData(symbolKey, "ES");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  set->setData(rateKey, "50");
  data.replace("ES", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Feeder Cattle"));
  set->setData(symbolKey, "FC");
  set->setData(rateKey, "500");
  set->setData(monthKey, "F,H,J,K,Q,U,V,X");
  set->setData(limitKey, "1.5");
  set->setData(exchangeKey, cme);
  data.replace("FC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Gold"));
  set->setData(symbolKey, "GC");
  set->setData(rateKey, "100");
  set->setData(monthKey, "G,J,M,Q,V,Z");
  set->setData(limitKey, "75");
  set->setData(exchangeKey, nymex);
  data.replace("GC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Goldman Sachs Commodity Index"));
  set->setData(symbolKey, "GI");
  set->setData(monthKey, "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData(exchangeKey, cme);
  data.replace("GI", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Copper"));
  set->setData(symbolKey, "HG");
  set->setData(rateKey, "250");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(limitKey, "20");
  set->setData(exchangeKey, nymex);
  data.replace("HG", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Heating Oil"));
  set->setData(symbolKey, "HO");
  set->setData(rateKey, "40000");
  set->setData(monthKey, "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData(limitKey, "6");
  set->setData(exchangeKey, nymex);
  data.replace("HO", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Unleaded Gasoline"));
  set->setData(symbolKey, "HU");
  set->setData(rateKey, "40000");
  set->setData(monthKey, "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData(limitKey, "6");
  set->setData(exchangeKey, nymex);
  data.replace("HU", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Frozen Concentrated Orange Juice"));
  set->setData(symbolKey, "OJ");
  set->setData(rateKey, "150");
  set->setData(monthKey, "F,H,K,N,U,X");
  set->setData(limitKey, "5");
  set->setData(exchangeKey, nybot);
  data.replace("OJ", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Frozen Concentrated Orange Juice"));
  set->setData(symbolKey, "OJ");
  set->setData(rateKey, "150");
  set->setData(monthKey, "F,H,K,N,U,X");
  set->setData(limitKey, "5");
  set->setData(exchangeKey, nybot);
  data.replace("JO", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Japanese Yen"));
  set->setData(symbolKey, "JY");
  set->setData(rateKey, "1250");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("JY", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Coffee"));
  set->setData(symbolKey, "KC");
  set->setData(rateKey, "375");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(exchangeKey, nybot);
  data.replace("KC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Lumber"));
  set->setData(symbolKey, "LB");
  set->setData(rateKey, "80");
  set->setData(monthKey, "F,H,K,N,U,X");
  set->setData(limitKey, "10");
  set->setData(exchangeKey, cme);
  data.replace("LB", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Live Cattle"));
  set->setData(symbolKey, "LC");
  set->setData(rateKey, "400");
  set->setData(monthKey, "G,J,M,Q,V,Z");
  set->setData(limitKey, "1.5");
  set->setData(exchangeKey, cme);
  data.replace("LC", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Lean Hogs"));
  set->setData(symbolKey, "LN");
  set->setData(rateKey, "400");
  set->setData(monthKey, "G,J,M,N,Q,V,Z");
  set->setData(limitKey, "2");
  set->setData(exchangeKey, cme);
  data.replace("LN", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Lean Hogs"));
  set->setData(symbolKey, "LN");
  set->setData(rateKey, "400");
  set->setData(monthKey, "G,J,M,N,Q,V,Z");
  set->setData(limitKey, "2");
  set->setData(exchangeKey, cme);
  data.replace("LH", set);
    
  set = new Setting;
  set->setData(nameKey, QObject::tr("NASDAQ 100"));
  set->setData(symbolKey, "ND");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("ND", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Natural Gas"));
  set->setData(symbolKey, "NG");
  set->setData(rateKey, "10000");
  set->setData(monthKey, "F,G,H,J,K,M,N,Q,U,V,X,Z");
  set->setData(limitKey, "1");
  set->setData(exchangeKey, nymex);
  data.replace("NG", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("E-MINI NASDAQ 100"));
  set->setData(symbolKey, "NQ");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("NQ", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Oats"));
  set->setData(symbolKey, "O");
  set->setData(rateKey, "50");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(limitKey, "20");
  set->setData(exchangeKey, cbot);
  data.replace("O", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Palladium"));
  set->setData(symbolKey, "PA");
  set->setData(rateKey, "100");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, nymex);
  data.replace("PA", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Frozen Pork Bellies"));
  set->setData(symbolKey, "PB");
  set->setData(rateKey, "400");
  set->setData(monthKey, "G,H,K,N,Q");
  set->setData(limitKey, "3");
  set->setData(exchangeKey, cme);
  data.replace("PB", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Platinum"));
  set->setData(symbolKey, "PL");
  set->setData(rateKey, "50");
  set->setData(monthKey, "F,J,N,V");
  set->setData(limitKey, "50");
  set->setData(exchangeKey, nymex);
  data.replace("PL", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Soybeans"));
  set->setData(symbolKey, "S");
  set->setData(rateKey, "50");
  set->setData(monthKey, "F,H,K,N,Q,U,X");
  set->setData(limitKey, "50");
  set->setData(exchangeKey, cbot);
  data.replace("S", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Sugar #11 World"));
  set->setData(symbolKey, "SB");
  set->setData(rateKey, "1120");
  set->setData(monthKey, "H,K,N,V");
  set->setData(exchangeKey, nybot);
  data.replace("SB", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Swiss Franc"));
  set->setData(symbolKey, "SF");
  set->setData(rateKey, "1250");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  data.replace("SF", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Silver"));
  set->setData(symbolKey, "SI");
  set->setData(rateKey, "100");
  set->setData(monthKey, "F,H,K,N,U,Z");
  set->setData(limitKey, "150");
  set->setData(exchangeKey, nymex);
  data.replace("SI", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Soy Meal"));
  set->setData(symbolKey, "SM");
  set->setData(rateKey, "100");
  set->setData(monthKey, "F,H,K,N,Q,U,V,Z");
  set->setData(limitKey, "20");
  set->setData(exchangeKey, cbot);
  data.replace("SM", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("S&P 500"));
  set->setData(symbolKey, "SP");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cme);
  set->setData(rateKey, "250");
  data.replace("SP", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Treasury Note 10 yr."));
  set->setData(symbolKey, "TY");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cbot);
  data.replace("TY", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("Treasury Note 10 yr."));
  set->setData(symbolKey, "TY");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cbot);
  data.replace("TYD", set);
    
  set = new Setting;
  set->setData(nameKey, QObject::tr("US Treasury Bond"));
  set->setData(symbolKey, "US");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cbot);
  data.replace("US", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("US Treasury Bond"));
  set->setData(symbolKey, "US");
  set->setData(rateKey, "1000");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, cbot);
  data.replace("USD", set);
    
  set = new Setting;
  set->setData(nameKey, QObject::tr("Wheat"));
  set->setData(symbolKey, "W");
  set->setData(rateKey, "50");
  set->setData(monthKey, "H,K,N,U,Z");
  set->setData(limitKey, "30");
  set->setData(exchangeKey, cbot);
  data.replace("W", set);

  set = new Setting;
  set->setData(nameKey, QObject::tr("NYSE"));
  set->setData(symbolKey, "YX");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, nybot);
  data.replace("YX", set);
  
  // Warsaw Stock Exchange futures
  set = new Setting;
  set->setData(nameKey, QObject::tr("WIG20 Index"));
  set->setData(symbolKey, "FW20");
  set->setData(monthKey, "H,M,U,Z");
  set->setData(exchangeKey, "WSE");
  set->setData(rateKey, "10");
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

