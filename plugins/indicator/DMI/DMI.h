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

class DMI : public IndicatorPlugin
{
  public:
    DMI ();
    virtual ~DMI ();
    void calculate ();
    void loadIndicatorSettings (QString);
    void saveIndicatorSettings (QString);
    int indicatorPrefDialog (QWidget *);
    void setDefaults();
    PlotLine * calculateCustom (QDict<PlotLine> *);
    QString getCustomSettings ();
    void setCustomSettings (QString);
    PlotLine * getMDI (int period);
    PlotLine * getPDI (int period);
    PlotLine * getADX (PlotLine *mdi, PlotLine *pdi, MAType maType, int period);
    
  private:
    QColor mdiColor;
    QColor pdiColor;
    QColor adxColor;
    PlotLine::LineType mdiLineType;
    PlotLine::LineType pdiLineType;
    PlotLine::LineType adxLineType;
    QString mdiLabel;
    QString pdiLabel;
    QString adxLabel;
    int period;
    int smoothing;
    IndicatorPlugin::MAType maType;
    QString lineRequest;
    QStringList lineList;
};

extern "C"
{
  Plugin * create ();
}
