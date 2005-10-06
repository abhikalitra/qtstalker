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

class AD : public IndicatorPlugin
{
  public:
    AD ();
    virtual ~AD ();
    void calculate ();
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    void calculateAD ();
    void calculateWAD ();
    void calculateCO ();
    void calculateCMF ();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    void getIndicatorSettings (Setting &);
    void setIndicatorSettings (Setting &);
    PlotLine * getAD ();
    int getMinBars ();
  
  private:
    QColor color;
    PlotLine::LineType lineType;
    QString label;
    QStringList methodList;
    QString method;
    int cmfPeriod;
};

extern "C"
{
  IndicatorPlugin * createIndicatorPlugin ();
}


