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
#include "SymbolLoad.h"
#include "GlobalControlPanel.h"
#include "GlobalParent.h"
#include "GlobalSymbol.h"
#include "ChartRefresh.h"
#include "KeySymbol.h"

ChartLoad::ChartLoad (QObject *p, QString symbol) : QObject (p)
{
  _symbol = symbol;
}

void ChartLoad::run ()
{
  SymbolLoad *sl = new SymbolLoad(this, _symbol);
  connect(sl, SIGNAL(signalDone(QString, int)), this, SLOT(symbolLoadDone(QString, int)));
  connect(sl, SIGNAL(signalError()), this, SLOT(deleteLater()));
  sl->start();
}

void ChartLoad::symbolLoadDone (QString title, int bars)
{
  KeySymbol keys;
  Data tds;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), tds);

  g_controlPanel->recentCharts()->addRecentChart(tds.toString());
  g_parent->setWindowTitle(title);
  g_controlPanel->setStart(bars, 0, 0);

  ChartRefresh *cr = new ChartRefresh(this);
  connect(cr, SIGNAL(signalDone()), this, SLOT(deleteLater()));
  cr->run();
}
