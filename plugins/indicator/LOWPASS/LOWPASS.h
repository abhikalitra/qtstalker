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

#define MAXNUM 2147483647

#if ! defined ( PI )
#define PI 3.141592653589793
#endif

#include "IndicatorPlugin.h"
#include "PlotLine.h"
#include "BarData.h"
#include "qtsFFT.h"

class LOWPASS : public IndicatorPlugin
{
  public:
    LOWPASS ();
    virtual ~LOWPASS ();
    void calculate ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *d) ;
    Setting getIndicatorSettings ();
    void setIndicatorSettings (Setting);
    
  private:
    QColor color;
    PlotLine::LineType lineType;
    double freq;
    double width;
    BarData::InputType input;
    QString label;
    QString customInput;

// Filter 

    void detrend(PlotLine* inLine, double* slope, double* intercept);
    PlotLine * detrend(PlotLine *x, double &slope, double &intercept, bool detrend = true);
    PlotLine * raise2Power(PlotLine *x, double pad = 0.0);
    qtsFFT * fft;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}




