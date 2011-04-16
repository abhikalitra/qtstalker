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

#ifndef PLUGIN_HISTOGRAM_DRAW_HPP
#define PLUGIN_HISTOGRAM_DRAW_HPP

#include <QColor>
#include <QString>
#include <QPainter>
#include <qwt_plot_curve.h>
#include <qwt_interval_data.h>
#include <qwt_text.h>
#include <qwt_scale_map.h>
#include <QList>

#include "Curve.h"

class HistogramDraw : public QwtPlotCurve
{
  public:
    explicit HistogramDraw (const QString &title = QString::null);
    explicit HistogramDraw (const QwtText &title);
    virtual ~HistogramDraw ();
    virtual QwtDoubleRect boundingRect () const;
    virtual int rtti () const;

    void init();
    void setCurveData (Curve *);
    int highLow (double &h, double &l);
    int highLowRange (int start, int end, double &h, double &l);
    int info (int, Setting *);

  private:
    Curve *_line;
    double _high;
    double _low;
};

#endif
