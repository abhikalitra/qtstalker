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

#include "CurveBar.h"
#include "Message.h"

class Curve : public QwtPlotCurve
{
  public:
    Curve ();
    ~Curve ();
    virtual int highLow (double &h, double &l);
    virtual int highLowRange (int start, int end, double &h, double &l);
    virtual int info (int, Message *);
    virtual void copy (Curve *);
    virtual QString toString ();
    virtual int fromString (QString);

    QwtDoubleRect boundingRect () const;
    int rtti () const;
    void init ();
    void setBar (int index, CurveBar *bar);
    CurveBar * bar (int index);
    void setType (QString);
    QString type ();
    void setLabel (QString text);
    QString & label ();
    int count ();
    void keys (QList<int> &);
    void keyRange (int &startIndex, int &endIndex);
    int setAllColor (QColor color);
    void deleteBar (int);
    void setPenWidth (int);
    int penWidth ();
    void setPlotName (QString);
    QString plotName ();

  protected:
    QHash<int, CurveBar *> _data;
    QString _label;
    QString _type;
    QString _plotName;
    int _penWidth;
    double _high;
    double _low;
};

#endif
