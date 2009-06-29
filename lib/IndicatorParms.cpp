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

#include "IndicatorParms.h"
#include <QObject>

IndicatorParms::IndicatorParms ()
{
  setPlot(0);
}

IndicatorParms::~IndicatorParms ()
{
}

void IndicatorParms::getString (QString &s)
{
  s.clear();
  QStringList l;
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key() + "=" + it.value());
  }
  s = l.join("|");
}

void IndicatorParms::parse (QString &d)
{
  dict.clear();

  QStringList l = d.split("|");

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QStringList l2 = l[loop].split("=");
    if (l2.count() > 1)
      dict.insert(l2[0], l2[1]);
  }
}

void IndicatorParms::getData (QString &k, QString &d)
{
  d.clear();
  if (dict.contains(k))
    d = dict.value(k);
}

void IndicatorParms::setData (QString &k, QString &d)
{
  dict.insert(k, d);
}

void IndicatorParms::setIndicator (QString &d)
{
  QString s("Indicator");
  setData(s, d);
}

void IndicatorParms::getIndicator (QString &d)
{
  QString s("Indicator");
  getData(s, d);
}

void IndicatorParms::setVariable (QString &d)
{
  QString s("Variable");
  setData(s, d);
}

void IndicatorParms::getVariable (QString &d)
{
  QString s("Variable");
  getData(s, d);
}

void IndicatorParms::setPlot (int d)
{
  QString s("Plot");
  QString s2 = QString::number(d);
  setData(s, s2);
}

int IndicatorParms::getPlot ()
{
  QString s("Plot");
  QString s2;
  getData(s, s2);
  return s2.toInt();
}

void IndicatorParms::setColor (QColor &d)
{
  QString s("Color");
  QString s2 = d.name();
  setData(s, s2);
}

void IndicatorParms::getColor (QColor &d)
{
  QString s("Color");
  QString s2;
  getData(s, s2);
  d.setNamedColor(s2);
}

void IndicatorParms::setLabel (QString &d)
{
  QString s("Label");
  setData(s, d);
}

void IndicatorParms::getLabel (QString &d)
{
  QString s("Label");
  getData(s, d);
}

void IndicatorParms::setLineType (QString &d)
{
  QString s("LineType");
  setData(s, d);
}

void IndicatorParms::getLineType (QString &d)
{
  QString s("LineType");
  getData(s, d);
}


