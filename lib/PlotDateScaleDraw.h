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

#ifndef PLOT_DATE_SCALE_DRAW_HPP
#define PLOT_DATE_SCALE_DRAW_HPP

#include <QHash>
#include <QDateTime>
#include <qwt_scale_draw.h>

#include "Bars.h"
#include "defs.h"

class QTSTALKER_SHARED_EXPORT PlotDateScaleDraw : public QwtScaleDraw
{
  public:
    PlotDateScaleDraw ();
    void clear ();
    void setDates ();
    virtual QwtText label (double) const;
    void info (int index, QStringList &);
    virtual void draw (QPainter *, const QPalette &) const;
    int startEndRange (int &start, int &end);
    int dateToX (QDateTime);

  private:
    QHash<QString, int> _dates;
};

#endif
