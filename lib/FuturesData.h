/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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


#ifndef FUTURESDATA_HPP
#define FUTURESDATA_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>
#include <qdatetime.h>
#include "Setting.h"

class FuturesData
{
  public:
    FuturesData ();
    ~FuturesData ();
    void loadData ();
    QString getName ();
    QString getSymbol ();
    QString getExchange ();
    QString getContract();
    float getLimit ();
    float getRate ();
    int setSymbol (QString &);
    void getMonthList (QStringList &);
    void getMonths (QStringList &);
    void getSymbolList (QString &, QStringList &);
    QString getCurrentContract (QDateTime &);

  protected:
    QString name;
    QString symbol;
    QString exchange;
    QString contract;
    float limit;
    QStringList monthList;
    float rate;
    QDict<Setting> data;
    QString nameKey;
    QString symbolKey;
    QString rateKey;
    QString monthKey;
    QString exchangeKey;
    QString limitKey;
    QString cme;
    QString cbot;
    QString nymex;
    QString nybot;
};

#endif
