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
#include "PlotFactory.h"

#include <QList>

IndicatorPlugin::IndicatorPlugin ()
{
  PlotFactory fac;
  fac.getList(plotList);

  BarData bd;
  bd.getInputFields(inputList);

  opList << "=" << "<" << "<=" << ">" << ">=";
  
  deleteFlag = 0;
}

IndicatorPlugin::~IndicatorPlugin ()
{
}

// virtual function
int IndicatorPlugin::getIndicator (Indicator &, BarData *)
{
  return 0;
}

// virtual function
int IndicatorPlugin::getCUS (QStringList &, QHash<QString, PlotLine *> &, BarData *)
{
  return 0;
}

// virtual function
int IndicatorPlugin::dialog (int)
{
  return 0;
}

//****************************************************
//*************** NON VIRTUAL FUNCTIONS **************
//****************************************************

void IndicatorPlugin::setSettings (Indicator &ind)
{
  ind.getSettings(settings);
  ind.getIndicator(indicator);
}

void IndicatorPlugin::getSettings (Indicator &ind)
{
  ind.setIndicator(indicator);
  ind.setSettings(settings);
}

void IndicatorPlugin::getIndicator (QString &d)
{
  d = indicator;
}

void IndicatorPlugin::getDialogSettings (PrefDialog *dialog)
{
  QList<int> keys;
  dialog->getKeyList(keys);

  int loop;
  for (loop = 0; loop < keys.count(); loop++)
  {
    QString d;
    dialog->getItem(keys[loop], d);
    if (! d.isEmpty())
      settings.setData(keys[loop], d);
  }
}

int IndicatorPlugin::getDeleteFlag ()
{
  return deleteFlag;
}

void IndicatorPlugin::getPlotList (QStringList &l, int flag)
{
  l = plotList;
  if (flag)
  {
    plotList.removeAll("Candle");
    plotList.removeAll("OHLC");
  }
}

