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

class MA3 : public IndicatorPlugin
{
  public:
    MA3 ();
    virtual ~MA3 ();
    void calculate ();
    QMemArray<int> getAlerts ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog ();
    void setDefaults();

  private:
    QColor fastColor;
    QColor midColor;
    QColor slowColor;
    PlotLine::LineType fastLineType;
    PlotLine::LineType midLineType;
    PlotLine::LineType slowLineType;
    QString fastLabel;
    QString midLabel;
    QString slowLabel;
    int fastPeriod;
    int midPeriod;
    int slowPeriod;
    IndicatorPlugin::MAType fastMAType;
    IndicatorPlugin::MAType midMAType;
    IndicatorPlugin::MAType slowMAType;
    IndicatorPlugin::InputType fastInput;
    IndicatorPlugin::InputType midInput;
    IndicatorPlugin::InputType slowInput;
    int fastDisplace;
    int midDisplace;
    int slowDisplace;
};

extern "C"
{
  Plugin * create ();
}


