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
#include "SymbolFunctions.h"
#include "WindowTitle.h"
#include "ChartRefresh.h"
#include "KeySymbol.h"
#include "GlobalSymbol.h"
#include "GlobalControlPanel.h"
#include "GlobalParent.h"

#include <QtDebug>

ChartLoad::ChartLoad ()
{
}

void ChartLoad::run (QString symbol)
{
  SymbolFunctions sf;
  if (sf.load(symbol))
    return;
  
  WindowTitle wt;
  KeySymbol keys;
  Data tds, length;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), tds);
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_LENGTH), length);

  g_controlPanel->recentCharts()->addRecentChart(tds.toString());
  
  g_parent->setWindowTitle(wt.mainWindowTitle(length.toInteger()));
  
  g_controlPanel->setStart(g_currentSymbol.ekeyCount(), 0, 0);

  ChartRefresh cr;
  cr.run();
}
