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

#ifndef PLUGIN_HPP
#define PLUGIN_HPP

#include <stdlib.h>
#include <qstring.h>
#include <qobject.h>
#include <qpixmap.h>
#include <qdict.h>
#include "PlotLine.h"
#include "BarData.h"
#include "Scaler.h"
#include "Setting.h"

class Plugin : public QObject
{
  public:
  
    enum PluginType
    {
      IndicatorPlug,
      QuotePlug,
      ChartPlug
    };
  
    Plugin ();
    virtual ~Plugin ();

    // indicator plugin interface
    virtual void setIndicatorInput (BarData *);
    virtual int getIndicatorLines ();
    virtual PlotLine * getIndicatorLine (int);
    virtual void calculate ();
    virtual void clearOutput ();
    virtual bool getPlotFlag ();
    virtual int indicatorPrefDialog (QWidget *);
    virtual void loadIndicatorSettings (QString);
    virtual void saveIndicatorSettings (QString);
    virtual PlotLine * calculateCustom (QDict<PlotLine> *);
    virtual Setting getIndicatorSettings ();
    virtual void setIndicatorSettings (Setting);
    virtual void setCustomFlag (bool);
    virtual void setCustomFunction (QString);

    // quote plugin interface
    virtual void update ();
    virtual void cancelUpdate ();
    virtual void prefDialog (QWidget *);
    
    // chart plugin interface
    virtual void drawChart (int, int, int);
    virtual void setChartInput (BarData *, Scaler *, QPixmap *);
    virtual int getMinPixelspace ();
    virtual int getStartX ();
    virtual bool getIndicatorFlag ();
    virtual void saveSettings ();
    
    // base plugin functions
    Plugin::PluginType getPluginType ();
    QString getPluginName ();
    void setDataPath (QString);

  protected:
    PluginType pluginType;
    QString pluginName;
    QString dataPath;
};

extern "C"
{
  Plugin * create ();
}

#endif


