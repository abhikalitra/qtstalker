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

#include "Chart.h"
#include "Globals.h"

#include <QtDebug>
#include <QObject>
#include <QString>

Chart::Chart ()
{
}

int Chart::run (IPCMessage mess, Data *dg)
{
  QString chart = dg->get("NAME")->toString();
  if (chart.isEmpty())
  {
    qDebug() << "Chart::run: invalid NAME" << chart;
    return 1;
  }

  Plot *plot = g_plotGroup->plot(chart);
  if (! plot)
  {
    plot = new Plot(chart, g_plotGroup);
    plot->setBarSpacing(g_controlPanel->barSpaceButton()->getPixelSpace());
    plot->loadSettings();
    plot->setScriptFile(mess.scriptFile());
    plot->setRow(dg->get("ROW")->toInteger());
    plot->setCol(dg->get("COL")->toInteger());

    QObject::connect(plot, SIGNAL(signalInfoMessage(Message)), g_infoPanel, SLOT(showInfo(Message)));
    QObject::connect(g_controlPanel->barSpaceButton(), SIGNAL(signalPixelSpace(int)), plot, SLOT(setBarSpacing(int)));
    QObject::connect(g_controlPanel, SIGNAL(signalValueChanged(int)), plot, SLOT(setStartIndex(int)));
    QObject::connect(plot, SIGNAL(signalIndex(int)), g_controlPanel, SLOT(setStartValue(int)));

    g_plotGroup->setPlot(plot);
  }

  plot->clear();

  plot->showDate(dg->get("DATE")->toBool());

  plot->setLogScaling(dg->get("LOG")->toBool());

  plot->update();

  return 0;
}
