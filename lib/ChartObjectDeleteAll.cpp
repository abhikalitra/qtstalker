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

#include "ChartObjectDeleteAll.h"
#include "DialogConfirm.h"
#include "GlobalPlotGroup.h"

#include <QtDebug>
#include <QStringList>

ChartObjectDeleteAll::ChartObjectDeleteAll (QObject *p, QString chart) : QObject (p)
{
  _chart = chart;
}

void ChartObjectDeleteAll::run ()
{
  QStringList mess;
  mess << tr("Confirm delete all chart objects from selected indicator") + "\n";
  mess << _chart;

  DialogConfirm *dialog = new DialogConfirm(0, QString(), Entity());
  dialog->setMessage(mess.join("\n"));
  connect(dialog, SIGNAL(accepted()), this, SLOT(remove2()));
  connect(dialog, SIGNAL(rejected()), this, SLOT(done()));
  dialog->show();
}

void ChartObjectDeleteAll::remove2 ()
{
  Plot *p = g_plotGroup->plot(_chart);
  if (! p)
  {
    done();
    return;
  }

  p->deleteAllChartObjects();

  done();
}

void ChartObjectDeleteAll::done ()
{
  deleteLater();
}
