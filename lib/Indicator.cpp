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

#include "Indicator.h"

#include <QObject>
#include <QStringList>



Indicator::Indicator ()
{
  enable = 0;
  tabRow = 1;
  date = 1;
  log = 0;
}

void Indicator::setName (QString &d)
{
  name = d;
}

void Indicator::getName (QString &d)
{
  d = name;
}

void Indicator::setEnable (int d)
{
  enable = d;
}

int Indicator::getEnable ()
{
  return enable;
}

void Indicator::setTabRow (int d)
{
  tabRow = d;
}

int Indicator::getTabRow ()
{
  return tabRow;
}

void Indicator::setDate (int d)
{
  date = d;
}

int Indicator::getDate ()
{
  return date;
}

void Indicator::setLog (int d)
{
  log = d;
}

int Indicator::getLog ()
{
  return log;
}

void Indicator::setParms (QString &d)
{
  parms.clear();

  if (d.isEmpty())
    return;

  QStringList l = d.split("\n");
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    IndicatorParms p;
    p.parse(l[loop]);
    parms.append(p);
  }
}

void Indicator::getParms (QString &d)
{
  int loop;
  QStringList l;
  for (loop = 0; loop < parms.count(); loop++)
  {
    IndicatorParms p = parms.at(loop);
    QString s;
    p.getString(s);
    l.append(s);
  }

  d = l.join("\n");
}

int Indicator::count ()
{
  return parms.count();
}

void Indicator::getParm (int i, IndicatorParms &p)
{
  p = parms.at(i);
}

void Indicator::appendParm (IndicatorParms &p)
{
  parms.append(p);
}

void Indicator::removeParm (int row)
{
  parms.removeAt(row);
}

void Indicator::setParm (int i, IndicatorParms &p)
{
  parms.replace(i, p);
}



