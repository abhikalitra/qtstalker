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

class MMA : public IndicatorPlugin
{
  public:
    MMA ();
    virtual ~MMA ();
    void calculate ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog ();
    void setDefaults();
    
  private:
    QColor fastColor;
    PlotLine::LineType fastLineType;
    QString fastLabel;
    QString fastLabel2;
    QString fastLabel3;
    QString fastLabel4;
    QString fastLabel5;
    QString fastLabel6;
    int fastPeriod;
    int fastPeriod2;
    int fastPeriod3;
    int fastPeriod4;
    int fastPeriod5;
    int fastPeriod6;
    int fastDisplace;
    int fastDisplace2;
    int fastDisplace3;
    int fastDisplace4;
    int fastDisplace5;
    int fastDisplace6;
    IndicatorPlugin::MAType fastMaType;
    IndicatorPlugin::InputType fastInput;
  
    QColor slowColor;
    PlotLine::LineType slowLineType;
    QString slowLabel;
    QString slowLabel2;
    QString slowLabel3;
    QString slowLabel4;
    QString slowLabel5;
    QString slowLabel6;
    int slowPeriod;
    int slowPeriod2;
    int slowPeriod3;
    int slowPeriod4;
    int slowPeriod5;
    int slowPeriod6;
    int slowDisplace;
    int slowDisplace2;
    int slowDisplace3;
    int slowDisplace4;
    int slowDisplace5;
    int slowDisplace6;
    IndicatorPlugin::MAType slowMaType;
    IndicatorPlugin::InputType slowInput;
    
    QColor longColor;
    PlotLine::LineType longLineType;
    QString longLabel;
    int longPeriod;
    int longDisplace;
    IndicatorPlugin::MAType longMaType;
    IndicatorPlugin::InputType longInput;
};

extern "C"
{
  Plugin * create ();
}


