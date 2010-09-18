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

#ifndef PLOT_HISTOGRAM_BAR_HPP
#define PLOT_HISTOGRAM_BAR_HPP

#include <QColor>
#include <QString>
#include <QPainter>
#include <QMap>
#include <qwt_plot_curve.h>
#include <qwt_interval_data.h>
#include <qwt_text.h>
#include <qwt_scale_map.h>

#include "Curve.h"

class PlotHistogramBar : public QwtPlotCurve
{
  public:
    enum HistogramAttribute
    {
      Auto = 0,
      Xfy = 1
    };

    explicit PlotHistogramBar (const QString &title = QString::null);
    explicit PlotHistogramBar (const QwtText &title);
    virtual ~PlotHistogramBar ();
    virtual QwtDoubleRect boundingRect () const;
    virtual int rtti () const;
    virtual void draw (QPainter *, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRect &) const;

    void init();
    void setData (Curve *);
    const QwtIntervalData & data () const;
    void setColor (const QColor &);
    QColor color () const;
    void setBaseline (double reference);
    double baseline () const;
    void setHistogramAttribute (HistogramAttribute, bool on = true);
    bool testHistogramAttribute (HistogramAttribute) const;

  protected:
    virtual void drawBar (QPainter *, Qt::Orientation o, const QRect &) const;

  private:
    class PrivateData;
    PrivateData *_data;
    QMap<int, QColor> _colors;
};

#endif
