/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "IndicatorPlotRule.h"

#include <QtDebug>

IndicatorPlotRule::IndicatorPlotRule ()
{
  _op = 0;
}

void IndicatorPlotRule::setOp (int d)
{
  _op = d;
}

int IndicatorPlotRule::op ()
{
  return _op;
}

void IndicatorPlotRule::setName (QString d)
{
  _name = d;
}

QString & IndicatorPlotRule::name ()
{
  return _name;
}

void IndicatorPlotRule::setValue (QString d)
{
  _value = d;
}

QString & IndicatorPlotRule::value ()
{
  return _value;
}

