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

#include "IndicatorIndex.h"


IndicatorIndex::IndicatorIndex ()
{
  enable = 0;
  tabRow = 1;
  date = 1;
  log = 0;
}

IndicatorIndex::~IndicatorIndex ()
{
}

void IndicatorIndex::setName (QString &d)
{
  name = d;
}

void IndicatorIndex::getName (QString &d)
{
  d = name;
}

void IndicatorIndex::setEnable (int d)
{
  enable = d;
}

int IndicatorIndex::getEnable ()
{
  return enable;
}

void IndicatorIndex::setTabRow (int d)
{
  tabRow = d;
}

int IndicatorIndex::getTabRow ()
{
  return tabRow;
}

void IndicatorIndex::setDate (int d)
{
  date = d;
}

int IndicatorIndex::getDate ()
{
  return date;
}

void IndicatorIndex::setLog (int d)
{
  log = d;
}

int IndicatorIndex::getLog ()
{
  return log;
}


