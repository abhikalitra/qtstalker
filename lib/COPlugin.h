/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef CO_PLUGIN_HPP
#define CO_PLUGIN_HPP

#include <QString>
#include <QPixmap>
#include <QSqlQuery>
#include <QIcon>
#include <QPainter>

#include "DateBar.h"
#include "Scaler.h"
#include "Setting.h"
#include "ChartObject.h"

class COPlugin
{
  public:
    COPlugin ();
    virtual ~COPlugin ();
    virtual void draw (ChartObject *, QPixmap &, DateBar &, int startX, int pixelspace,
		       int startIndex, Scaler &);
    virtual int getHighLow (ChartObject *);
    virtual void getInfo (ChartObject *, Setting *);
    virtual void dialog (ChartObject *);
    virtual void setSettings (ChartObject *, QSqlQuery &q);
    virtual void getSettings (ChartObject *, QString &);
    virtual void create (ChartObject *);
    virtual int create2 (ChartObject *, QDateTime &, double);
    virtual int create3 (ChartObject *, QDateTime &, double);
    virtual void moving (ChartObject *, QDateTime &, double, int moveFlag);
    virtual void getIcon (QIcon &);
    virtual int inDateRange (ChartObject *, QDateTime &, QDateTime &);

  protected:
    int handleWidth;
};

#endif
