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

#include "IndicatorPlugin.h"

class COMP : public IndicatorPlugin
{
  public:
    COMP ();
    virtual ~COMP ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    int getMinBars ();
    void calculateIf ();
    void calculateAndOr ();
    void calculateThen ();
  
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    QString data1;
    QString data2;
    QString data3;
    QString data4;
    QString data5;
    QString data6;
    QString method;
    QString method2;
    QString delay1;
    QString delay2;
    QString delay3;
    QString delay4;
    QString andor;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


