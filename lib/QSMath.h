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

#ifndef QSMATH_HPP
#define QSMATH_HPP

#include "PlotLine.h"
#include "BarData.h"
#include <qstringlist.h>

class QSMath
{
  public:
    enum MAType
    {
      EMA,
      SMA,
      WMA,
      Wilder
    };
    
    QSMath();
    QSMath(BarData *d);
    ~QSMath();
    QStringList getMATypes ();
    PlotLine * getMA (PlotLine *, MAType, int, int);
    PlotLine * getMA (PlotLine *, MAType, int);
    PlotLine * getSMA (PlotLine *, int);
    PlotLine * getEMA (PlotLine *, int);
    PlotLine * getWilderMA (PlotLine *, int);
    PlotLine * getWMA (PlotLine *, int);
    PlotLine * getTR (); // true range
    PlotLine * getTP (); // typical price H+L+C/3
    PlotLine * getAD ();    // accumulation distribution
    // bollinger band: flag=0 lower band, flag=1 upper band
    PlotLine * getBB (MAType maType, int period, double deviation, int flag);
    PlotLine * getCCI (int period); // cci
    PlotLine * getPDI (int period); //plus directional index 
    PlotLine * getMDI (int period); //minus directional index 
    PlotLine * getADX (PlotLine *mdi, PlotLine *pdi, MAType maType, int period); // average directional index
    PlotLine * getOSC (PlotLine *d, MAType fastMaType, MAType slowMaType,
                       int fastPeriod, int slowPeriod); // oscillator
    PlotLine * getMF (int period); // money flow
    PlotLine * getMOM (PlotLine *in, int period); // momentum
    PlotLine * getNVI (); //negative volume index
    PlotLine * getOBV (); //on balance volume
    PlotLine * getPC (PlotLine *in, int period); //pecent change
    PlotLine * getPER (PlotLine *in); //performance
    double getPPFR (double high, double low, double close); // pivot point first resistance
    double getPPSR (double high, double low, double close); // pivot point second resistance
    double getPPTR (double high, double low, double close); // pivot point third resistance
    double getPPFS (double high, double low, double close); // pivot point first support
    double getPPSS (double high, double low, double close); // pivot point second support
    double getPPTS (double high, double low, double close); // pivot point third support
    PlotLine * getPVI (); //positive volume index
    PlotLine * getPVT (); //positive volume trend
    PlotLine * getROC (PlotLine *in, int period); //rate of change
    PlotLine * getRSI (PlotLine *in, int period); //relative strength index
    PlotLine * getSAR (double initial, double add, double limit); // parabolic support and resistance
    PlotLine * getSD (PlotLine *in, int period); // standard deviance
    PlotLine * getSTOCH (MAType maType, int period, int kperiod); // stochastics
    PlotLine * getVFI (int period); //volume flow index
    PlotLine * getVOLR (int period); //volatility ratio
    PlotLine * getWAD (); //william's accumulation distribution
    PlotLine * getWILLR (int period); //william's %R
    
  protected:
    BarData *data;
};

#endif

