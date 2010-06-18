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

#ifndef FUTURES_HPP
#define FUTURES_HPP

#include "DBPlugin.h"
#include "BarData.h"
#include "QuoteIndexDataBase.h"

class Futures : public DBPlugin
{
  public:
    enum ScriptMethod
    {
      SET_QUOTE,
      SET_NAME,
      SET_CODE,
      SET_MONTH,
      SET_YEAR,
      SAVE_QUOTES,
      DELETE,
      GET_QUOTES,
      RENAME
    };
    
    Futures ();
    void getBars (BarData &);
    void setBars ();
    int createTable (BarData *, QuoteIndexDataBase &);
    int scriptCommand (QStringList &, Indicator &);
    int deleteSymbol (BarData *);
    
    int scriptSetQuote (QStringList &);
    int scriptSetName (QStringList &);
    int scriptSetCode (QStringList &);
    int scriptSetMonth (QStringList &);
    int scriptSetYear (QStringList &);
    int scriptSaveQuotes (QStringList &l);
    int scriptDelete (QStringList &l);
    int scriptGetQuotes (QStringList &, Indicator &);
    int scriptRename (QStringList &l);

  protected:
    QStringList scriptMethods;
};

extern "C"
{
  DBPlugin * createDBPlugin ();
}

#endif
