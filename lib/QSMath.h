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
#include <qdict.h>

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
    
  public:
    enum Operator
    {
      NoOp,
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual
    };
    
    QSMath();
    QSMath(BarData *d);
    ~QSMath();
    QStringList getMATypes ();
    QSMath::MAType getMAType (QString);
    QStringList QSMath::getOperatorTypes ();
    QSMath::Operator getOperator (QString);

    PlotLine * getMA (PlotLine *, MAType, int, int);
    PlotLine * getMA (PlotLine *, MAType, int);
    PlotLine * getSMA (PlotLine *, int);
    PlotLine * getEMA (PlotLine *, int);
    PlotLine * getWilderMA (PlotLine *, int);
    PlotLine * getWMA (PlotLine *, int);
    QString getMA2 (int, QStringList);
    
    PlotLine * getTR (); // true range
    QString getTR2 (int i, QStringList);
    
    PlotLine * getTP (); // typical price H+L+C/3
    QString getTP2 (int i, QStringList);
    
    PlotLine * getAD ();    // accumulation distribution
    QString getAD2 (int i, QStringList);
    
    // bollinger band: flag=0 lower band, flag=1 upper band
    PlotLine * getBB (MAType maType, int period, double deviation, int flag);
    QString getBB2 (int i, QStringList);
    
    PlotLine * getCCI (int period); // cci
    QString getCCI2 (int i, QStringList);
    
    PlotLine * getMDI (int period); //minus directional index 
    QString getMDI2 (int i, QStringList);
    
    PlotLine * getPDI (int period); //plus directional index
    QString getPDI2 (int i, QStringList);
    
    PlotLine * getADX (PlotLine *mdi, PlotLine *pdi, MAType maType, int period); // average directional index
    QString getADX2 (int i, QStringList);
    
    PlotLine * getOSC (PlotLine *d, MAType fastMaType, MAType slowMaType,
                       int fastPeriod, int slowPeriod); // oscillator
    QString getOSC2 (int i, QStringList);
    
    PlotLine * getMF (int period); // money flow
    QString getMF2 (int i, QStringList);
    
    PlotLine * getMOM (PlotLine *in, int period); // momentum
    QString getMOM2 (int i, QStringList);
    
    PlotLine * getNVI (); //negative volume index
    QString getNVI2 (int i, QStringList);
    
    PlotLine * getOBV (); //on balance volume
    QString getOBV2 (int i, QStringList);

    PlotLine * getPC (PlotLine *in, int period); //pecent change
    QString getPC2 (int i, QStringList);
    
    PlotLine * getPER (PlotLine *in); //performance
    QString getPER2 (int i, QStringList);
    
    double getPPFR (double high, double low, double close); // pivot point first resistance
    double getPPSR (double high, double low, double close); // pivot point second resistance
    double getPPTR (double high, double low, double close); // pivot point third resistance
    double getPPFS (double high, double low, double close); // pivot point first support
    double getPPSS (double high, double low, double close); // pivot point second support
    double getPPTS (double high, double low, double close); // pivot point third support
    
    PlotLine * getPVI (); //positive volume index
    QString getPVI2 (int i, QStringList);
    
    PlotLine * getPVT (); //positive volume trend
    QString getPVT2 (int i, QStringList);
    
    PlotLine * getROC (PlotLine *in, int period); //rate of change
    QString getROC2 (int i, QStringList);
    
    PlotLine * getRSI (PlotLine *in, int period); //relative strength index
    QString getRSI2 (int i, QStringList);
    
    PlotLine * getSAR (double initial, double add, double limit); // parabolic support and resistance
    QString getSAR2 (int i, QStringList);
    
    PlotLine * getSD (PlotLine *in, int period); // standard deviance
    QString getSD2 (int i, QStringList);
    
    PlotLine * getSTOCH (MAType maType, int period, int kperiod); // stochastics
    QString getSTOCH2 (int i, QStringList);
    
    PlotLine * getVFI (int period); //volume flow index
    QString getVFI2 (int i, QStringList);
    
    PlotLine * getVOLR (int period); //volatility ratio
    QString getVOLR2 (int i, QStringList);
    
    PlotLine * getWAD (); //william's accumulation distribution
    QString getWAD2 (int i, QStringList);
    
    PlotLine * getWILLR (int period); //william's %R
    QString getWILLR2 (int i, QStringList);
    
    QString getMUL (int i, QStringList); // multiply plotline
    QString getDIV (int i, QStringList); // divide plotline
    QString getSUB (int i, QStringList); // subtract plotline
    QString getADD (int i, QStringList); // add plotline
    QString getREF (int i, QStringList); // days back or current data array
    QString getCOMPARE (int i, QStringList); // compares 2 data arrays and returns boolean array
    
    // custom functions
    void clearCustomLines ();
    PlotLine * getCustomLine (int);
    QString calculateCustomFormula (QStringList, QStringList);
    PlotLine * getInputLine (QString);
    
  protected:
    BarData *data;
    QDict<PlotLine> customLines;
};

#endif

