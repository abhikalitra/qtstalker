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

#include "CommandChart.h"
#include "GlobalPlotGroup.h"
#include "GlobalControlPanel.h"
#include "GlobalInfoPanel.h"
#include "DataString.h"
#include "DataBool.h"
#include "DataInteger.h"

#include <QtDebug>

CommandChart::CommandChart (QObject *p) : Command (p)
{
  _name = "CHART";
  _type = _WAIT;

  _values.insert(_ParmTypeName, new DataString());
  _values.insert(_ParmTypeDate, new DataBool(TRUE));
  _values.insert(_ParmTypeLog, new DataBool(FALSE));
  _values.insert(_ParmTypeRow, new DataInteger(0));
  _values.insert(_ParmTypeCol, new DataInteger(99));
}

void CommandChart::runScript (CommandParse sg, Script *script)
{
  if (Command::parse(sg, script))
  {
    Command::error("CommandChart::runScript: parse error");
    return;
  }

  chart(_values.value(_ParmTypeName)->toString(),
	script->file(),
	_values.value(_ParmTypeRow)->toInteger(),
	_values.value(_ParmTypeCol)->toInteger(),
	_values.value(_ParmTypeDate)->toBool(),
	_values.value(_ParmTypeLog)->toBool());

  Command::done(QString());
}

void CommandChart::chart (QString chart, QString script, int row, int col, bool date, bool log)
{
  Plot *plot = g_plotGroup->plot(chart);
  if (! plot)
  {
    plot = new Plot(chart, g_plotGroup);
    plot->setBarSpacing(g_controlPanel->barSpaceButton()->getPixelSpace());
    plot->loadSettings();
    plot->setScriptFile(script);
    plot->setRow(row);
    plot->setCol(col);

    connect(plot, SIGNAL(signalInfoMessage(Message)), g_infoPanel, SLOT(showInfo(Message)));
    connect(g_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
    connect(g_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
    connect(plot, SIGNAL(signalIndex(int)), g_controlPanel, SLOT(setStartValue(int)));

    g_plotGroup->setPlot(plot);
  }

  plot->showDate(date);
  plot->setLogScaling(log);
}
