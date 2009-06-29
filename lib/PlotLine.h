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

#ifndef PLOTLINE_HPP
#define PLOTLINE_HPP

#include <QString>
#include <QList>
#include <QColor>
#include <QDateTime>
#include "Setting.h"


typedef struct
{
  double v;
  double open;
  double high;
  double low;
  QColor color;
  bool candleFill;

} Val;


class PlotLine
{
  public:
  
    enum LineType
    {
      Dot,
      Dash,
      Histogram,
      HistogramBar,
      Line,
      Invisible,
      Horizontal,
      Bar,
      Candle
    };
  
    PlotLine ();
    ~PlotLine ();
    void copy (PlotLine *);
    void setColor (QString &);
    void setColor (QColor &);
    void getColor (QColor &);
    void setType (PlotLine::LineType);
    void setType (QString &);
    PlotLine::LineType getType ();
    void setLabel (QString &);
    void getLabel (QString &);
    void append (double);
    void append (QColor &, double, double, double, double, bool);
    void append2 (QColor &, double, double, double, double, bool);
    void append (QDateTime &);
    void prepend (QDateTime &);
    void prepend (double);
    void prepend (QColor &, double, double, double, double, bool);
    double getData (int);
    void getData (int, QColor &, double &, double &, double &, double &, bool &);
    void getData (int, QDateTime &);
    void setData (int, double);
    int getSize ();
    double getHigh ();
    void setHigh (double);
    double getLow ();
    void setLow (double);
    void checkHighLow (double);
    void setScaleFlag (bool);
    bool getScaleFlag ();
    void setColorFlag (bool);
    bool getColorFlag ();
    void getLineTypes (QStringList &);
    void setColorBar (int, QColor &);
    void getColorBar (int, QColor &);
    void getHighLowRange (int, int, double &, double &);
    void getInfo (int, Setting &);
    void strip (double, int, QString &);
    void getDateList (QList<QDateTime> &);
    void setDateList (QList<QDateTime> &);
    void getData (QList<Val> &);
    
  private:
    QList<Val> data;
    QList<QDateTime> dateList;
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    double high;
    double low;
    bool scaleFlag;
    bool colorFlag;
};

#endif

