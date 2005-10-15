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
#include "Config.h"
#include <qfile.h>
#include <qtextstream.h>

IndicatorPlugin::IndicatorPlugin()
{
  minBars = 2;
  enabled = TRUE;
  
  output = new Indicator;
  saveFlag = FALSE;
  plotType = 1;
  
  PlotLine *pl = new PlotLine;
  pl->getLineTypes(lineTypes);
  delete pl;

  BarData *it = new BarData;
  it->getInputFields(inputTypeList);
  delete it;
  
  opList.append("EQ");
  opList.append("LT");
  opList.append("LTEQ");
  opList.append("GT");
  opList.append("GTEQ");
  opList.append("AND");
  opList.append("OR");
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

void IndicatorPlugin::clearOutput ()
{
  output->clearLines();
}

void IndicatorPlugin::loadFile (QString &file, Setting &dict)
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

void IndicatorPlugin::saveFile (QString &file, Setting &dict)
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
  
  QStringList key;
  dict.getKeyList(key);
  
  int loop;
  for(loop = 0; loop < (int) key.count(); loop++)
    stream << key[loop] << "=" << dict.getData(key[loop]) << "\n";
  
  f.close();
}

Indicator * IndicatorPlugin::getIndicator ()
{
  return output;
}

QStringList IndicatorPlugin::getMATypes ()
{
  QStringList l;
  Config config;
  QString s("MA");
  IndicatorPlugin *plug = config.getIndicatorPlugin(s);
  if (! plug)
  {
    qDebug("IndicatorPlugin::getMATypes: cannot open MA plugin");
    config.closePlugin(s);
    return l;
  }
  
  l = plug->getMATypes();
  config.closePlugin(s);
  
  return l;
}

PlotLine * IndicatorPlugin::getMA (PlotLine *in, int type, int period)
{
  PlotLine *ma = 0;
  Config config;
  QString s("MA");
  IndicatorPlugin *plug = config.getIndicatorPlugin(s);
  if (! plug)
  {
    qDebug("IndicatorPlugin::getMA: cannot open MA plugin");
    config.closePlugin(s);
    return ma;
  }

  ma = plug->getMA(in, type, period);
  config.closePlugin(s);
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

IndicatorPlugin::Operator IndicatorPlugin::getOperator (QString d)
{
  Operator op = NoOp;
  
  while (1)
  {
    if (! d.compare("EQ"))
    {
      op = Equal;
      break;
    }
    
    if (! d.compare("LT"))
    {
      op = LessThan;
      break;
    }
  
    if (! d.compare("LTEQ"))
    {
      op = LessThanEqual;
      break;
    }
      
    if (! d.compare("GT"))
    {
      op = GreaterThan;
      break;
    }
    
    if (! d.compare("AND"))
    {
      op = And;
      break;
    }
    
    if (! d.compare("OR"))
    {
      op = Or;
      break;
    }
    
    if (! d.compare("GTEQ"))
      op = GreaterThanEqual;
    
    break;
  }
  
  return op;
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

PlotLine * IndicatorPlugin::calculateCustom (QString &, QPtrList<PlotLine> &)
{
  return 0;
}

void IndicatorPlugin::getIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setIndicatorSettings (Setting &)
{
}

void IndicatorPlugin::setCustomFunction (QString &)
{
}

int IndicatorPlugin::getMinBars ()
{
  return minBars;
}

void IndicatorPlugin::saveIndicatorSettings (QString &d)
{
  Setting set;
  getIndicatorSettings(set);
  saveFile(d, set);
}

void IndicatorPlugin::loadIndicatorSettings (QString &d)
{
  Setting set;
  loadFile(d, set);
  setIndicatorSettings(set);
}

