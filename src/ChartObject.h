/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef CHARTOBJECT_HPP
#define CHARTOBJECT_HPP

#include "Scaler.h"
#include "Setting.h"
#include "BarData.h"
#include <qpixmap.h>
#include <qobject.h>
#include <qstring.h>

class ChartObject : public QObject
{
  Q_OBJECT

  public:
    ChartObject ();
    virtual ~ChartObject ();
    virtual void draw (Scaler &, QPixmap &, int, int);
    virtual QString getDate ();
    virtual QString getDate2 ();
    void setData (QString);
    
  protected:
    Setting settings;
    BarData *data;
};

#endif
