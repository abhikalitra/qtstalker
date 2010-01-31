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


Indicator::Indicator ()
{
  enable = 0;
  tabRow = 1;
  date = 1;
  log = 0;
  cus = 0;
}

void Indicator::setName(QString &d)
{
  name = d;
}

void Indicator::getName(QString &d)
{
  d = name;
}

void Indicator::setEnable(int d)
{
  enable = d;
}

int Indicator::getEnable()
{
  return enable;
}

void Indicator::setTabRow(int d)
{
  tabRow = d;
}

int Indicator::getTabRow()
{
  return tabRow;
}

void Indicator::setDate(int d)
{
  date = d;
}

int Indicator::getDate()
{
  return date;
}

void Indicator::setCUS(int d)
{
  cus = d;
}

int Indicator::getCUS()
{
  return cus;
}

void Indicator::setLog(int d)
{
  log = d;
}

int Indicator::getLog()
{
  return log;
}

void Indicator::setIndicator(QString &d)
{
  indicator = d;
  if (d == "CUS")
    cus = 1;
}

void Indicator::getIndicator(QString &d)
{
  d = indicator;
}

void Indicator::setLines(QList<PlotLine *> &d)
{
  lines = d;
}

void Indicator::getLines(QList<PlotLine *> &d)
{
  d = lines;
}

void Indicator::setSettings(Setting &d)
{
  settings = d;
}

void Indicator::getSettings(Setting &d)
{
  d = settings;
}

void Indicator::addLine(PlotLine *d)
{
  lines.append(d);
}

