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
#include "QSMath.h"
#include <qstring.h>
#include <qlist.h>
#include <qmemarray.h>
#include <qdict.h>

class IndicatorPlugin : public Plugin
{
  public:
    IndicatorPlugin();
    virtual ~IndicatorPlugin();
    QMemArray<int> getAlerts ();
    void setIndicatorInput (BarData *);
    int getIndicatorLines ();
    void clearOutput ();
    bool getPlotFlag ();
    bool getAlertFlag ();
    QDict<QString> loadFile (QString);
    void saveFile (QString, QDict<QString>);
    PlotLine * getIndicatorLine (int);

  protected:
    BarData *data;
    QList<PlotLine> output;
    QMemArray<int> alerts;
    QStringList lineTypes;
    QStringList maTypeList;
    QStringList inputTypeList;
    bool plotFlag;
    bool alertFlag;
    bool saveFlag;
};

#endif

