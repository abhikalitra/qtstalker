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

#include "IndicatorPlugin.h"
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>

IndicatorPlugin::IndicatorPlugin()
{
  minBars = 2;
  enabled = TRUE;
  
  output = new Indicator;
  saveFlag = FALSE;
  customFlag = FALSE;
  plotType = 1;
  
  PlotLine *pl = new PlotLine;
  lineTypes = pl->getLineTypes();
  delete pl;

  BarData *it = new BarData;
  inputTypeList = it->getInputFields();
  delete it;
}

IndicatorPlugin::~IndicatorPlugin()
{
  delete output;
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
{
  data = d;
  output->clearLines();
}

void IndicatorPlugin::setCustomFlag (bool d)
{
  customFlag = d;
}

void IndicatorPlugin::clearOutput ()
{
  output->clearLines();
}

void IndicatorPlugin::loadFile (QString file, Setting &dict)
{
  output->clearLines();

  QFile f(file);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("IndicatorPlugin:can't read file %s", file.latin1());
    return;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    
    if (l.count() < 2)
      continue;
      
    if (l.count() > 2)
    {
      QString k = l[0];
      s = s.remove(0, k.length() + 1);
      dict.setData(k, s);  
    }
    else
      dict.setData(l[0], l[1]);  
  }
  
  f.close();
  
  QString s = dict.getData("plotType");
  if (s.length())
    plotType = s.toInt();

  s = dict.getData("enable");
  if (s.length())
    enabled = s.toInt();
}

void IndicatorPlugin::saveFile (QString file, Setting &dict)
{
  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("IndicatorPlugin:can't save file %s", file.latin1());
    return;
  }
  QTextStream stream(&f);
  
  dict.setData("plotType", QString::number(plotType));
  dict.setData("enable", QString::number(enabled));
  
  QStringList key = dict.getKeyList();
  
  int loop;
  for(loop = 0; loop < (int) key.count(); loop++)
    stream << key[loop] << "=" << dict.getData(key[loop]) << "\n";
  
  f.close();
}

Indicator * IndicatorPlugin::getIndicator ()
{
  return output;
}

PlotLine * IndicatorPlugin::getInputLine (QString d)
{
  PlotLine *in = 0;
  
  if (d.contains("#"))
    return in;
      
  bool ok;
  d.toInt(&ok, 10);
  if (ok)
    in = customLines->find(d);

  return in;
}

QStringList IndicatorPlugin::getMATypes ()
{
  QStringList l;
  Config config;
  IndicatorPlugin *plug = config.getIndicatorPlugin("MA");
  if (! plug)
  {
    qDebug("IndicatorPlugin::getMATypes: cannot open MA plugin");
    config.closePlugin("MA");
    return l;
  }
  
  l = plug->getMATypes();
  config.closePlugin("MA");
  
  return l;
}

PlotLine * IndicatorPlugin::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = 0;
  Config config;
  IndicatorPlugin *plug = config.getIndicatorPlugin("MA");
  if (! plug)
  {
    qDebug("IndicatorPlugin::getMA: cannot open MA plugin");
    config.closePlugin("MA");
    return ma;
  }

  ma = plug->getMA(in, type, period);
  config.closePlugin("MA");
  return ma;  
}

void IndicatorPlugin::setPlotType (int d)
{
  plotType = d;
}

QString IndicatorPlugin::getPluginName ()
{
  return pluginName;
}

QString IndicatorPlugin::getHelpFile ()
{
  return helpFile;
}

//***************************************************************
//****************** VIRTUAL OVERIDES ***************************
//***************************************************************

void IndicatorPlugin::calculate ()
{
}

int IndicatorPlugin::indicatorPrefDialog (QWidget *)
{
  return 0;
}

PlotLine * IndicatorPlugin::calculateCustom (QDict<PlotLine> *)
{
  return 0;
}

void IndicatorPlugin::getIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setCustomFunction (QString)
{
}

int IndicatorPlugin::getMinBars ()
{
  return minBars;
}

void IndicatorPlugin::saveIndicatorSettings (QString d)
{
  Setting set;
  getIndicatorSettings(set);
  saveFile(d, set);
}

void IndicatorPlugin::loadIndicatorSettings (QString d)
{
  Setting set;
  loadFile(d, set);
  setIndicatorSettings(set);
}


