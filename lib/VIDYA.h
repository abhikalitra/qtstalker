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

#include "IndicatorPlugin.h"


class VIDYA : public IndicatorPlugin
{
  public:
    VIDYA ();
    virtual ~VIDYA ();
    Indicator * calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QString &, QPtrList<PlotLine> &);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    void formatDialog (QStringList &vl, QString &rv, QString &rs);
    void calcVidya ( PlotLine *outSignal, PlotLine *inSignal, int iCmoPeriod, int iVidyaPeriod );
	
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    int period;
    int volPeriod;
    IndicatorPlugin *plug;
    BarData::InputType input;
	
    //! utilities -- should be called math :-)
	
    void getStdDev( PlotLine *outLine, PlotLine *inLine,  int iPeriod );
    void getNorm( PlotLine *inSig, double iMin, double iMax );
    void calcCMO ( PlotLine *outSignal, PlotLine *inSignal, int iPeriod);
    void calcAdaptCMO ( PlotLine *outSignal, PlotLine *inSignal, int iStdPeriod, int iMinLook, int iMaxLook);

    QString colorLabel;
    QString labelLabel;
    QString lineTypeLabel;
    QString pluginLabel;
    QString periodLabel;
    QString volPeriodLabel;
    QString inputLabel;
};

