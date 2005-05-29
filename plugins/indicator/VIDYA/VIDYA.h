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


class VIDYA : public IndicatorPlugin
{
  public:
    VIDYA ();
    virtual ~VIDYA ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *d);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    int getMinBars ();
	
    //! Programatic interface
    void calcVidya ( PlotLine *outSignal, PlotLine *inSignal, int iCmoPeriod, int iVidyaPeriod );
	
  
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    QString customInput;
    int period;
    int volPeriod;
    IndicatorPlugin *plug;
	
    //! utilities -- should be called math :-)
	
    void getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod );
    void getNorm( PlotLine *inSig, double iMin, double iMax );
    void calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod);
    void calcAdaptCMO ( PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod, int iMinLook, int iMaxLook);
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

