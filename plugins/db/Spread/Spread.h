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

#ifndef SPREAD_HPP
#define SPREAD_HPP

#include "DbPlugin.h"
#include "Bar.h"
#include "Setting.h"
#include <qstring.h>
#include <qdict.h>

class Spread : public DbPlugin
{
  public:
    Spread ();
    ~Spread ();
    BarData * getHistory ();
    Bar * getBar (QString, QString);
    void dbPrefDialog ();
    void setBar (Bar *);
    void updateSpread ();
    void loadData (QString, QString);
    QString createNew ();
    void saveDbDefaults (Setting *);

  private:
    QDict<Bar> data;
};

extern "C"
{
  Plugin * create ();
}

#endif
