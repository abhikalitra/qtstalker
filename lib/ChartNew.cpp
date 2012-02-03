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

#include "ChartNew.h"
#include "GlobalPlotGroup.h"
#include "GlobalControlPanel.h"
#include "GlobalInfoPanel.h"
#include "KeyIndicatorDataBase.h"
#include "Global.h"
#include "IndicatorFunctions.h"

#include <QtDebug>
#include <QObject>

ChartNew::ChartNew ()
{
}

int ChartNew::run (Entity &e)
{
  Data name, command, file, date, log, row, col;
  e.toData(QString("NAME"), name);
  e.toData(QString("COMMAND"), command);
  e.toData(QString("FILE"), file);
  e.toData(QString("DATE"), date);
  e.toData(QString("LOG"), log);
  e.toData(QString("ROW"), row);
  e.toData(QString("COL"), col);
  return run(name.toString(), command.toString(), file.toString(), row.toInteger(),
	     col.toInteger(), date.toBool(), log.toBool());
}

int ChartNew::run (QString chart, QString command, QString script, int row, int col, bool date, bool log)
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

    QObject::connect(plot, SIGNAL(signalInfoMessage(Entity)), g_infoPanel, SLOT(showInfo(Entity)));
    QObject::connect(g_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
    QObject::connect(g_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
    QObject::connect(plot, SIGNAL(signalIndex(int)), g_controlPanel, SLOT(setStartValue(int)));

    g_plotGroup->setPlot(plot);

    KeyIndicatorDataBase keys;
    Entity i;
    i.setName(chart);
    i.set(keys.indexToString(KeyIndicatorDataBase::_SESSION), Data(g_session));
    i.set(keys.indexToString(KeyIndicatorDataBase::_FILE), Data(script));
    i.set(keys.indexToString(KeyIndicatorDataBase::_COMMAND), Data(command));

    IndicatorFunctions ifunc;
    if (ifunc.set(i))
      qDebug() << "ChartNew::run: indicators db error";
  }

  plot->showDate(date);
  plot->setLogScaling(log);
  
  return 0;
}
