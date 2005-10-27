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

#include "Indicator.h"
#include <qobject.h>

Indicator::Indicator ()
{
  lines.setAutoDelete(TRUE);
  enable = TRUE;
  tabRow = 0;
  logScale = FALSE;
  dateFlag = TRUE;
}

Indicator::~Indicator ()
{
}

int Indicator::getLines ()
{
  return (int) lines.count();
}

PlotLine * Indicator::getLine (int i)
{
  return lines.at(i);
}

void Indicator::addLine (PlotLine *l)
{
  lines.append(l);
}

void Indicator::setTabRow (int d)
{
  tabRow = d;
}

int Indicator::getTabRow ()
{
  return tabRow;
}

void Indicator::clearLines ()
{
  lines.clear();
}

void Indicator::setEnable (bool status)
{
  enable = status;
}

bool Indicator::getEnable ()
{
  return enable;
}

void Indicator::setName (QString &d)
{
  name = d;
}

QString Indicator::getName ()
{
  return name;
}

void Indicator::setType (QString &d)
{
  type = d;
}

QString Indicator::getType ()
{
  return type;
}

void Indicator::setFile (QString &d)
{
  file = d;
}

QString Indicator::getFile ()
{
  return file;
}

void Indicator::copy (Indicator *d)
{
  int loop;
  for (loop = 0; loop < d->getLines(); loop++)
  {
    PlotLine *tpl = d->getLine(loop);
    PlotLine *pl = new PlotLine;
    pl->copy(tpl);
    addLine(pl);
  }

//  setDateFlag(d->getDateFlag());
//  setLogScale(d->getLogScale());
//  setTabRow(d->getTabRow());
//  setEnable(d->getEnable());
//  setName(d->getName());
//  setFile(d->getFile());
//  setType(d->getType());
}

void Indicator::setDateFlag (bool d)
{
  dateFlag = d;
}

bool Indicator::getDateFlag ()
{
  return dateFlag;
}

void Indicator::setLogScale (bool d)
{
  logScale = d;
}

bool Indicator::getLogScale ()
{
  return logScale;
}

