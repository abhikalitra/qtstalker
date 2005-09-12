/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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


class CMO : public IndicatorPlugin
{
  public:
    CMO ();
    virtual ~CMO ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *d);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    int getMinBars ();
    bool adaptFlag;
    int minLookback;
    int maxLookback;
	
    //! Programatic interface
    void calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod);
    void calcAdaptCMO ( PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod, int iMinLook, int iMaxLook);
  
  private:
    QColor color;
    QColor buyColor;
    QColor sellColor;
    PlotLine::LineType lineType;
    QString label;
    QString customInput;
    int period;
    int buyLine;
    int sellLine;
	
    //! utilities -- should be called math :-)
    void getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod );
    void getNorm( PlotLine *inSig, double iMin, double iMax );
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}
