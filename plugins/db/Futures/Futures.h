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

#ifndef STOCKS_HPP
#define STOCKS_HPP

#include "DbPlugin.h"
#include "BarDate.h"
#include "Bar.h"
#include <qstring.h>

class Futures : public DbPlugin
{
  public:
    Futures ();
    ~Futures ();
    Bar * getBar (QString, QString);
    void dbPrefDialog ();
    void setBar (BarDate date, double open, double high, double low, double close, double volume, double);
    void saveDbDefaults (BarData::BarType barType, QString symbol, QString name, QString futuresType,
                         QString futuresMonth, QString, QString);

  private:
};

extern "C"
{
  Plugin * create ();
}

#endif
