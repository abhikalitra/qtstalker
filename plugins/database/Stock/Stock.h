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

#ifndef STOCK_HPP
#define STOCK_HPP

#include "DBPlugin.h"
#include "BarData.h"
#include "QuoteIndexDataBase.h"
#include "StockDetailsDataBase.h"

class Stock : public DBPlugin
{
  public:
    enum ScriptMethod
    {
      SET_QUOTE,
      SAVE_QUOTES,
      DELETE,
      GET_QUOTES,
      RENAME,
      SET_DETAIL,
      GET_DETAIL
    };
    
    Stock ();
    void getBars (BarData &);
    void setBars ();
    int createTable (BarData *, QuoteIndexDataBase &);
    int deleteSymbol (BarData *);
    int setDetail (QString &key, BarData *bd, QString &data);
    int setDetail (int key, BarData *bd, QString &data);
    int detail (QString &key, BarData *bd, QString &data);
    int detail (int key, BarData *bd, QString &data);

    int scriptCommand (QStringList &, Indicator &, QByteArray &);
    int scriptSetQuote (QStringList &);
    int scriptSaveQuotes (QStringList &l);
    int scriptDelete (QStringList &l);
    int scriptGetQuotes (QStringList &l, Indicator &);
    int scriptRename (QStringList &l);
    int scriptSetDetail (QStringList &);
    int scriptGetDetail (QStringList &, QByteArray &);
    
  protected:
    QStringList scriptMethods;
    StockDetailsDataBase _ddb;
};

extern "C"
{
  DBPlugin * createDBPlugin ();
}

#endif
