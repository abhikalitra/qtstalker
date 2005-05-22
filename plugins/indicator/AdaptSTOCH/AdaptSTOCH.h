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

class AdaptSTOCH : public IndicatorPlugin
{
  public:
    AdaptSTOCH ();
    virtual ~AdaptSTOCH ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    int getMinBars ();
    
  private:
    QColor dcolor;
    QColor kcolor;
    QColor buyColor;
    QColor sellColor;
    PlotLine::LineType dlineType;
    PlotLine::LineType klineType;
    QString dlabel;
    QString klabel;
    int dperiod;
    int kperiod;
    int period;
	int minLookback;
  	int maxLookback;
	int kMaType;
	int dMaType;
    int buyLine;
    int sellLine;
    QString customInput;
    QString label;
	
	PlotLine *getHighest( PlotLine *line, int period);
	PlotLine *getLowest( PlotLine *line,  int period);
	PlotLine *getStdDev( PlotLine *line,  int period );
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}



