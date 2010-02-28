/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef PLUGIN_FACTORY_HPP
#define PLUGIN_FACTORY_HPP

#include "IndicatorPlugin.h"
#include "COPlugin.h"
#include "PlotPlugin.h"

#include <QString>
#include <QStringList>
#include <QHash>
#include <QLibrary>

class PluginFactory
{
  public:
    PluginFactory ();
    ~PluginFactory ();
    void setup ();
    IndicatorPlugin * getIndicator (QString &indicator);
    IndicatorPlugin * getIndicatorPlugin (QString &indicator, QString &file);
    COPlugin * getCO (QString &co);
    COPlugin * getCOPlugin (QString &co, QString &file);
    PlotPlugin * getPlot (QString &co);
    PlotPlugin * getPlotPlugin (QString &co, QString &file);
    
  protected:
    QStringList notPluginList;
    QHash<QString, QLibrary *> libs;
    QHash<QString, IndicatorPlugin *> indicatorPlugins;
    QHash<QString, COPlugin *> coPlugins;
    QHash<QString, PlotPlugin *> plotPlugins;
};

#endif
