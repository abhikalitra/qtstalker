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
#include "Globals.h"
#include "Symbol.h"
#include "QuoteDataBase.h"
#include "SharedMemory.h"
#include "IndicatorDataBase.h"
#include "Script.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "SettingDateTime.h"

#include <QStringList>
#include <QVariant>
#include <QSettings>

ChartLoad::ChartLoad (QString symbol)
{
  _symbol = symbol;
}

int ChartLoad::run ()
{
  // do all the stuff we need to do to load a chart

  QStringList tl = _symbol.split(":");
  if (tl.count() != 2)
    return 1;

  g_currentSymbol->clear();
  g_currentSymbol->set(Symbol::_EXCHANGE, new SettingString(tl.at(0)));
  g_currentSymbol->set(Symbol::_SYMBOL, new SettingString(tl.at(1)));
  g_currentSymbol->set(Symbol::_LENGTH, new SettingInteger(g_controlPanel->barLengthButton()->length()));
  g_currentSymbol->set(Symbol::_START_DATE, new SettingDateTime(QDateTime()));
  g_currentSymbol->set(Symbol::_END_DATE, new SettingDateTime(QDateTime()));
  g_currentSymbol->set(Symbol::_RANGE, new SettingInteger(g_controlPanel->dateRangeControl()->dateRange()));

  QuoteDataBase db;
  if (db.getBars(g_currentSymbol))
  {
    qDebug() << "ChartLoad::run: QuoteDataBase error";
    return 1;
  }

//qDebug() << g_currentSymbol->toString();

  SharedMemory sm;
  if (sm.setData(g_sharedCurrentSymbol, g_currentSymbol->toString()))
  {
    qDebug() << "ChartLoad::run: error creating shared memory";
    return 1;
  }

  // update settings file
  QSettings settings(g_localSettings);
  settings.setValue("current_symbol", _symbol);
  settings.sync();

  g_controlPanel->setStart(g_currentSymbol->barKeyCount(), 0, 0);

  // run indicators
  IndicatorDataBase i;
  QStringList il = i.indicators();
  int loop = 0;
  for (; loop < il.count(); loop++)
  {
    QString command;
    if (i.indicator(il.at(loop), command))
      continue;

    Script script(0);
    script.setFile(il.at(loop));
    script.setCommand(command);
    script.setSession(g_session);
    script.run();
  }

  g_sidePanel->setBusyFlag(0);

  return 0;
}
