/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#ifndef INDICATORPLUGIN_HPP
#define INDICATORPLUGIN_HPP

#include "Plugin.h"
#include "PlotLine.h"
#include "Setting.h"
#include <qstring.h>
#include <qptrlist.h>
#include <qdict.h>

class IndicatorPlugin : public Plugin
{
  public:

    enum MAType
    {
      EMA,
      SMA,
      WMA,
      Wilder
    };
  
    IndicatorPlugin();
    virtual ~IndicatorPlugin();
    void setIndicatorInput (BarData *);
    int getIndicatorLines ();
    void clearOutput ();
    void setCustomFlag (bool);
    Setting loadFile (QString);
    void saveFile (QString, Setting);
    PlotLine * getIndicatorLine (int);
    PlotLine * getInputLine (QString);
    PlotLine * getEMA (PlotLine *d, int);
    PlotLine * getSMA (PlotLine *d, int);
    PlotLine * getWMA (PlotLine *d, int);
    PlotLine * getWilderMA (PlotLine *d, int);
    PlotLine * getMA (PlotLine *d, IndicatorPlugin::MAType, int);
    QStringList getMATypes ();
    IndicatorPlugin::MAType getMAType (QString);
    void setPlotType (int);

  protected:
    BarData *data;
    QPtrList<PlotLine> output;
    QStringList lineTypes;
    QStringList inputTypeList;
    bool saveFlag;
    bool customFlag;
    QDict<PlotLine> *customLines;
    QStringList maTypeList;
    int plotType;
};

#endif

