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
#include "VerifyDataInput.h"

#include <QtDebug>

CommandChart::CommandChart (QObject *p) : Command (p)
{
  _name = "CHART";
  _type = _NORMAL;
}

int CommandChart::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // verify name
  QString name;
  QString s = sg->value("NAME");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandChart::runScript: invalid NAME" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // verify DATE
  bool date = TRUE;
  s = sg->value("DATE");
  if (vdi.toBool(script, s, date))
  {
    qDebug() << "CommandChart::runScript: invalid DATE, using default" << s;
    date = TRUE;
  }

  // verify LOG
  bool log = FALSE;
  s = sg->value("LOG");
  if (vdi.toBool(script, s, log))
  {
    qDebug() << "CommandChart::runScript: invalid LOG, using default" << s;
    log = FALSE;
  }

  // verify ROW
  int row = 0;
  s = sg->value("ROW");
  if (vdi.toInteger(script, s, row))
  {
    qDebug() << "CommandChart::runScript: invalid ROW, using default" << s;
    row = 0;
  }

  // verift COL
  int col = 99;
  s = sg->value("COL");
  if (vdi.toInteger(script, s, col))
  {
    qDebug() << "CommandChart::runScript: invalid COL, using default" << s;
    col = 99;
  }

  chart(name, script->file(), row, col, date, log);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
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
