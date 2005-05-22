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

class LMS : public IndicatorPlugin
{
  public:
    LMS ();
    virtual ~LMS ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
  
  private:
    QColor colorK;
    QColor color2Day;
    QColor color5Day;
    PlotLine::LineType lineTypeK;
    PlotLine::LineType lineType2Day;
    PlotLine::LineType lineType5Day;
    QString label;

    PlotLine *slowK (PlotLine *inLine, int kPeriod, int slowKperiod);
	
    bool show2Day;
    bool show5Day;
    bool cycleFlag;
    int fkPeriod;
    int skPeriod;
    int cmbIndex;
	
    // ---------------------------------
    bool testFlag; // test only --------
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}

