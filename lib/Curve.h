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

#ifndef CURVE_HPP
#define CURVE_HPP

#include <QStringList>
#include <QHash>
#include <QColor>
#include <QPainter>
#include <QList>
#include <qwt_plot_curve.h>
#include <qwt_interval_data.h>
#include <qwt_text.h>
#include <qwt_scale_map.h>

#include "Entity.h"

class Curve : public QwtPlotCurve
{
  public:
    Curve ();
    ~Curve ();
    virtual int highLow (double &h, double &l);
    virtual int highLowRange (int start, int end, double &h, double &l);
    virtual int info (int, Entity &);
    virtual int scalePoint (int, QColor &, double &);

    QwtDoubleRect boundingRect () const;
    int rtti () const;
    void init ();
    void setSettings (Entity &);
    Entity & settings ();
    void keys (QList<int> &);
    void keyRange (int &startIndex, int &endIndex);
    int setAllColor (QColor color);

  protected:
    mutable Entity _settings;
    double _high;
    double _low;
};

#endif
