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

#ifndef HORIZONTALLINE_HPP
#define HORIZONTALLINE_HPP

#include "ChartObject.h"

class HorizontalLine : public ChartObject
{
  Q_OBJECT

  public:
    HorizontalLine (Scaler *, QPixmap *, QString, QString, double);
    ~HorizontalLine ();
    void draw (int, int);
    void move (QDateTime, double);
    Setting * getSettings ();
    void setSettings (Setting *);
    
  public slots:
    void prefDialog ();
    
  protected:
};

#endif