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

#ifndef PLOTLINE_HPP
#define PLOTLINE_HPP

#include <qstring.h>
#include <qptrlist.h>
#include <qobject.h>
#include <qpixmap.h>
#include "Scaler.h"

typedef struct
{
  double v;

} Val;

class PlotLine : public QObject
{
  Q_OBJECT

  public:
  
    enum LineType
    {
      Dot,
      Dash,
      Histogram,
      HistogramBar,
      Line,
      Invisible,
      Horizontal
    };
  
    PlotLine ();
    ~PlotLine ();
    void copy (PlotLine *);
    void setColor (QString &);
    void setColor (QColor &);
    QColor getColor ();
    void setType (PlotLine::LineType);
    void setType (QString &);
    PlotLine::LineType getType ();
    void setLabel (QString &);
    QString getLabel ();
    void append (double);
    void prepend (double);
    double getData (int);
    void setData (int, double);
    int getSize ();
    double getHigh ();
    void setHigh (double);
    double getLow ();
    void setLow (double);
    void checkHighLow (double);
    void setScaleFlag (bool);
    bool getScaleFlag ();
    void getLineTypes (QStringList &);

    void setColorFlag (bool);
    bool getColorFlag ();
    void appendColorBar (QColor &);
    void prependColorBar (QColor &);
    QColor getColorBar (int);

    void draw (int, int, int, int);
    void drawLine (int, int, int, int);
    void drawHorizontalLine (int);
    void drawHistogram (int, int, int, int);
    void drawHistogramBar (int, int, int, int);
    void drawDot (int, int, int, int);
    QString strip (double);
    void setPointers (Scaler *, QPixmap *);
    
  private:
    Scaler *scaler;
    QPixmap *buffer;
    QPtrList<Val> data;
    QPtrList<QColor> colorBars;
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    double high;
    double low;
    bool colorBarFlag;
    bool scaleFlag;
};

#endif

