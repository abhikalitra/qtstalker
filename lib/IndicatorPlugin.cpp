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
#include <qfile.h>
#include <qtextstream.h>

IndicatorPlugin::IndicatorPlugin()
{
  output.setAutoDelete(TRUE);
  pluginType = IndicatorPlug;
  saveFlag = FALSE;
  plotFlag = FALSE;
  alertFlag = FALSE;
  
  PlotLine *pl = new PlotLine;
  lineTypes = pl->getLineTypes();
  delete pl;
  
  QSMath *m = new QSMath;
  maTypeList = m->getMATypes();
  delete m;
  
  BarData *it = new BarData;
  inputTypeList = it->getInputFields();
  delete it;
}

IndicatorPlugin::~IndicatorPlugin()
{
}

void IndicatorPlugin::setIndicatorInput (BarData *d)
{
  data = d;
  output.clear();
}

QMemArray<int> IndicatorPlugin::getAlerts ()
{
  alerts.fill(0, data->count());
  return alerts;
}

bool IndicatorPlugin::getPlotFlag ()
{
  return plotFlag;
}

bool IndicatorPlugin::getAlertFlag ()
{
  return alertFlag;
}

void IndicatorPlugin::clearOutput ()
{
  output.clear();
}

QDict<QString> IndicatorPlugin::loadFile (QString file)
{
  output.clear();

  QDict<QString>dict;
  dict.setAutoDelete(TRUE);
  
  QFile f(file);
  if (! f.open(IO_ReadOnly))
  {
    qDebug("IndicatorPlugin:can't read file %s", file.latin1());
    return dict;
  }
  QTextStream stream(&f);
  
  while(stream.atEnd() == 0)
  {
    QString s = stream.readLine();
    s = s.stripWhiteSpace();
    if (! s.length())
      continue;
      
    QStringList l = QStringList::split("=", s, FALSE);
    if (l.count() != 2)
      continue;

    dict.replace(l[0], new QString(l[1]));      
  }
  
  f.close();
  
  return dict;
}

void IndicatorPlugin::saveFile (QString file, QDict<QString> dict)
{
  QFile f(file);
  if (! f.open(IO_WriteOnly))
  {
    qDebug("IndicatorPlugin:can't save file %s", file.latin1());
    return;
  }
  QTextStream stream(&f);
  
  QDictIterator<QString> it(dict);
  for(; it.current(); ++it)
  {
    QString *s = it.current();
    stream << it.currentKey() << "=" << s->left(s->length()) << "\n";
  }
  
  f.close();
}

int IndicatorPlugin::getIndicatorLines ()
{
  return (int) output.count();
}

PlotLine * IndicatorPlugin::getIndicatorLine (int d)
{
  PlotLine *line = output.at(d);
  if (line)
    return line;
  else
    return 0;
}



