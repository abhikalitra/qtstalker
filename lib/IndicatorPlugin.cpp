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

#include "IndicatorPlugin.h"

#include <QDebug>

IndicatorPlugin::IndicatorPlugin ()
{
//  _opList << "=" << "<" << "<=" << ">" << ">=";
  _deleteFlag = 0;
}

IndicatorPlugin::~IndicatorPlugin ()
{
}

// virtual function
int IndicatorPlugin::getIndicator (Indicator &, BarData &)
{
  return 0;
}

// virtual function
int IndicatorPlugin::getCUS (QStringList &, Indicator &, BarData &)
{
  return 0;
}

// virtual function
IndicatorPluginDialog * IndicatorPlugin::dialog (Indicator &)
{
  return 0;
}

// virtual function
void IndicatorPlugin::defaults (Indicator &)
{
}

//****************************************************
//*************** NON VIRTUAL FUNCTIONS **************
//****************************************************

QString & IndicatorPlugin::indicator ()
{
  return _indicator;
}

int IndicatorPlugin::deleteFlag ()
{
  return _deleteFlag;
}

