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

#ifndef FUTURESDATA_HPP
#define FUTURESDATA_HPP

#include <qstring.h>
#include <qstringlist.h>

#define SYMBOLS "AD,BO,C,CC,CD,CL,CR,CT,DJ,DX,EC,ED,ES,FC,GC,GI,HG,HO,HU,JO,JY,KC,LB,LC,LN,NB,ND,NG,NQ,O,PA,PB,PL,S,SB,SF,SI,SM,SP,TY,US,W"

class FuturesData
{
  public:
    FuturesData ();
    ~FuturesData ();
    QString getName ();
    QString getSymbol ();
    QString getExchange ();
    QString getLimit ();
    QString getCC ();
    float getRate ();
    int setSymbol (QString);
    QStringList getMonthList ();
    QStringList getMonths ();
    QStringList getSymbolList ();
    void ccMonth1 (int);
    void ccMonth2 (int);
    void ccMonth3 (int);

  protected:
    QString name;
    QString symbol;
    QString exchange;
    QString limit;
    QString cc;
    QStringList monthList;
    float rate;
    int ccFlag;
};

#endif
