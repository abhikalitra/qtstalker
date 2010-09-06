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
#include <QMap>
#include <QColor>

#include "CurveBar.h"
#include "Setting.h"

class Curve
{
  public:
    enum Type
    {
      Candle,
      Dash,
      Dot,
      Histogram,
      HistogramBar,
      Horizontal,
      Line,
      OHLC
    };
    
    Curve ();
    Curve (Curve::Type);
    ~Curve ();
    void init ();
    void setBar (int index, CurveBar *bar);
    CurveBar * bar (int index);
    void setType (Curve::Type);
    Curve::Type & type ();
    void setLabel (QString &text);
    QString & label ();
    int count ();
    int setAllColor (QColor &color);
    void setZ (int);
    int z ();
    void keys (QList<int> &);
    void keyRange (int &startIndex, int &endIndex);
    void setColor (QColor &);
    QColor & color ();
    void list (QStringList &, int);
    int typeFromString (QString &);
    void info (int, Setting &);
    int highLowRange (int start, int end, double &h, double &l);

  protected:
    QStringList _list;
    QMap<int, CurveBar *> _data;
    QString _label;
    Curve::Type _type;
    int _z;
    QColor _color;
};

#endif

