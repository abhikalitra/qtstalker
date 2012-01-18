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

// *************************************************************************************************
// Plots the date area of a chart
// *************************************************************************************************

#ifndef DATE_SCALE_DRAW_HPP
#define DATE_SCALE_DRAW_HPP

#include <QDateTime>
#include <QList>
#include <QHash>
#include <qwt_scale_draw.h>

#include "Entity.h"

class DateScaleDraw : public QwtScaleDraw
{
  public:
    DateScaleDraw ();
    void clear ();
    void setDates ();
    void setDates (Entity &);
    int count ();
    virtual QwtText label (double) const;
    void date (int, QDateTime &);
    int x (QDateTime d);
    void info (int index, Entity &);
    QList<QDateTime> & dates ();
    virtual void draw (QPainter *, const QPalette &) const;

  private:
    QHash<QString, int> _data;
    QHash<int, QDateTime> _dates;
    QList<QDateTime> _dateList;
    int _barLength;
    mutable QString _dateString;
};

#endif


