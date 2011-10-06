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


#include "ChartRefresh.h"
#include "Global.h"
#include "GlobalPlotGroup.h"
#include "GlobalSymbol.h"
#include "QuoteDataBase.h"
#include "IndicatorDataBase.h"

#include <QStringList>

ChartRefresh::ChartRefresh (QObject *p) : QObject (p)
{
}

ChartRefresh::~ChartRefresh ()
{
  emit signalDone();
}

int ChartRefresh::run ()
{
  // do all the stuff we need to do to load a chart

  if (! g_chartRefreshMutex.tryLock())
  {
    deleteLater();
    return 1;
  }

  IndicatorDataBase i;
  QStringList il = i.indicators();

  // move displayed plots to the top of the list
  // so they update first for user speed
  QStringList ft = g_plotGroup->frontTabs();
  int loop = 0;
  for (; loop < ft.count(); loop++)
  {
    int i = il.indexOf(ft.at(loop));
    if (i == -1)
      continue;

    il.move(i, 0);
  }

  for (loop = 0; loop < il.count(); loop++)
  {
    QString command;
    if (i.indicator(il.at(loop), command))
      continue;

    Script *script = new Script(this);
    connect(script, SIGNAL(signalDeleted(QString)), this, SLOT(scriptDone(QString)));
    script->setFile(il.at(loop));
    script->setCommand(command);
    script->setSymbol(g_currentSymbol);
    script->run();

    _scripts.insert(script->id(), script);
  }

  return 0;
}

void ChartRefresh::scriptDone (QString d)
{
  _scripts.remove(d);

  if (_scripts.count())
    return;

  g_chartRefreshMutex.unlock();

  deleteLater();
}
