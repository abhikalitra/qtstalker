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

#ifndef PLOTLINE_HPP
#define PLOTLINE_HPP

#include <QString>
#include <QMap>
#include <QList>

#include "PlotData.h"
#include "Scaler.h"
#include "PlotLineBar.h"
#include "Setting.h"
#include "PlotStyle.h"

class PlotLine
{
  public:
    PlotLine ();
    ~PlotLine ();
    void draw (PlotData &, Scaler &);
    void info (int, Setting *);
    void setType (QString &);
    QString & type ();
    void setLabel (QString &);
    QString & label ();
    PlotLineBar * data (int);
    void setData (int, double);
    void setData (int, double, QColor &);
    void setData (int, PlotLineBar *);
    int count ();
    void setPlotFlag (bool);
    bool plotFlag ();
    int highLowRange (int, int, double &, double &);
    void keys (QList<int> &);
    void keyRange (int &, int &);
    void setColor (QString &);

  protected:
    QMap<int, PlotLineBar *> _data;
    QString _label;
    QString _type;
    bool _plotFlag;
    PlotStyle *_plotStyle;
};

#endif

