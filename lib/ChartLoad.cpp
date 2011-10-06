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


#include "ChartLoad.h"
#include "Global.h"
#include "GlobalSymbol.h"
#include "GlobalControlPanel.h"
#include "GlobalPlotGroup.h"
#include "GlobalParent.h"
#include "GlobalSidePanel.h"
#include "QuoteDataBase.h"
#include "IndicatorDataBase.h"
#include "BarLength.h"

#include <QStringList>
#include <QSettings>

ChartLoad::ChartLoad (QObject *p, QString symbol) : QObject (p)
{
  _symbol = symbol;
}

ChartLoad::~ChartLoad ()
{
//  g_symbolMutex.unlock();
}

int ChartLoad::run ()
{
  // do all the stuff we need to do to load a chart

  if (! g_symbolMutex.tryLock())
  {
    deleteLater();
    return 1;
  }

  if (! g_currentSymbol)
    g_currentSymbol = new Symbol;

  g_currentSymbol->clear();
  if (g_currentSymbol->setSymbolFull(_symbol))
  {
    qDebug() << "ChartLoad::run: invalid symbol" << _symbol;
    g_symbolMutex.unlock();
    deleteLater();
    return 1;
  }

  g_currentSymbol->setLength(g_controlPanel->barLengthButton()->length());
  g_currentSymbol->setRange(g_controlPanel->dateRangeControl()->dateRange());

  QuoteDataBase db;
  if (db.getBars(g_currentSymbol))
  {
    qDebug() << "ChartLoad::run: QuoteDataBase error";
    g_symbolMutex.unlock();
    deleteLater();
    return 1;
  }

//qDebug() << g_currentSymbol->toString();

  // update settings file
  QSettings settings(g_localSettings);
  settings.setValue("current_symbol", _symbol);
  settings.sync();

  g_controlPanel->setStart(g_currentSymbol->barKeyCount(), 0, 0);

  // run indicators
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

    Script *script = new Script(g_parent);
    connect(script, SIGNAL(signalDeleted(QString)), this, SLOT(scriptDone(QString)));
    script->setFile(il.at(loop));
    script->setCommand(command);
    script->setSymbol(g_currentSymbol);
    script->run();
    _scripts.insert(script->id(), script);
  }

  g_controlPanel->recentCharts()->addRecentChart(_symbol);

  g_parent->setWindowTitle(getWindowCaption());

  return 0;
}

void ChartLoad::scriptDone (QString d)
{
  _scripts.remove(d);

  if (_scripts.count())
    return;

  g_symbolMutex.unlock();

  deleteLater();
}

QString ChartLoad::getWindowCaption ()
{
  // update the main window text
  QStringList l;
  l << "QtStalker" + g_session + ":";

  if (! g_currentSymbol)
    return l.join(" ");

  QString name = g_currentSymbol->name();
  if (! name.isEmpty())
    l << name;

  QString symbol = g_currentSymbol->symbol();
  if (! symbol.isEmpty())
    l << "(" + symbol + ")";

  QStringList bl;
  BarLength b;
  bl = b.list();
  l << bl.at(g_controlPanel->barLengthButton()->length());

  return l.join(" ");
}
