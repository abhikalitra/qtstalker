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

#include "PlotLine.h"
#include "Setting.h"
#include "BarData.h"
#include "Indicator.h"
#include <qstring.h>
#include <qstringlist.h>
#include <qdict.h>

class IndicatorPlugin
{
  public:
  
    enum Operator
    {
      NoOp,
      Equal,
      LessThan,
      LessThanEqual,
      GreaterThan,
      GreaterThanEqual,
      And,
      Or
    };
  
    IndicatorPlugin();
    virtual ~IndicatorPlugin();
    void setIndicatorInput (BarData *);
    Indicator * getIndicator ();
    void clearOutput ();
    void setCustomFlag (bool);
    void loadFile (QString &, Setting &);
    void saveFile (QString &, Setting &);
    PlotLine * getInputLine (QString &);
    void setPlotType (int);
    QString getPluginName ();
    QString getHelpFile ();
    IndicatorPlugin::Operator getOperator (QString);

    virtual void calculate ();
    virtual int indicatorPrefDialog (QWidget *);
    virtual PlotLine * calculateCustom (QDict<PlotLine> *);
    virtual void getIndicatorSettings (Setting &);
    virtual void setIndicatorSettings (Setting &);
    virtual void setCustomFunction (QString &);
    virtual int getMinBars ();
    virtual void loadIndicatorSettings (QString &);
    virtual void saveIndicatorSettings (QString &);
    virtual PlotLine * getMA (PlotLine *d, int, int);
    virtual QStringList getMATypes ();
    
  protected:
    BarData *data;
    Indicator *output;
    QStringList lineTypes;
    QStringList inputTypeList;
    QStringList opList;
    bool saveFlag;
    bool customFlag;
    QDict<PlotLine> *customLines;
    int plotType;
    QString pluginName;
    QString helpFile;
    int minBars;
    bool enabled;
};

#endif

