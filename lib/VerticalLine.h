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

#ifndef VERTICALLINE_HPP
#define VERTICALLINE_HPP


#include "BaseCO.h"

class VerticalLine : public BaseCO
{
  Q_OBJECT

  public:
    VerticalLine ();
    void draw (QPixmap &buffer, BarData *data, int startX, int pixelspace, int startIndex, Scaler &);
    void getInfo (Setting &);
    void dialog ();
    void setDate (QDateTime &);
    void setSettings (QSqlQuery &q);
    void getSettings (QString &);
    void create (QString &_id, QString &_symbol, QString &_indicator);
    int create2 (QDateTime &x, double y);
    void moving (QDateTime &x, double y, int moveFlag);

  protected:
    QDateTime date;
};

#endif
