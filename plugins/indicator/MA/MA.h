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

class MA : public IndicatorPlugin
{
  public:
    enum MAType
    {
      EMA,
      SMA,
      WMA,
      Wilder
    };
  
    MA ();
    virtual ~MA ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    int getMinBars ();
    PlotLine * getEMA (PlotLine *d, int);
    PlotLine * getSMA (PlotLine *d, int);
    PlotLine * getWMA (PlotLine *d, int);
    PlotLine * getWilderMA (PlotLine *d, int);
    PlotLine * getMA (PlotLine *d, int, int);
    QStringList getMATypes ();
    int getMAType (QString);
    
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    int period;
    int maType;
    BarData::InputType input;
    QString customInput;
    QStringList maTypeList;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


