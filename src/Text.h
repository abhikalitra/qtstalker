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

#ifndef TEXT_HPP
#define TEXT_HPP

#include "ChartObject.h"
#include <qfont.h>

class Text : public ChartObject
{
  Q_OBJECT

  public:
    Text (Scaler *, QPixmap *, QString, QString, BarDate, double);
    ~Text ();
    void draw (int, int);
    void move (BarDate, double);
    Setting * getSettings ();
    void setSettings (Setting *);
    
  public slots:
    void prefDialog ();
    
  protected:
    QString label;
    QFont textFont;
};

#endif
