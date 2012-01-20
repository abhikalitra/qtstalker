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
#include "EAVDataBase.h"
#include "EAVSearch.h"
#include "KeyIndicatorDataBase.h"
#include "GlobalSidePanel.h"

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
  KeyIndicatorDataBase keys;
  EAVSearch search;
  search.append(keys.indexToString(KeyIndicatorDataBase::_SESSION), "=", g_session);
  
  EAVDataBase db("indicators");
  QStringList il;
  db.search(search, il);

  // move displayed plots to the top of the list
  // so they update first for user speed
  QStringList ft = g_plotGroup->frontTabs();
  int loop = 0;
  for (; loop < ft.size(); loop++)
  {
    int i = il.indexOf(ft.at(loop));
    if (i == -1)
      continue;

    il.move(i, 0);
  }

  for (loop = 0; loop < il.size(); loop++)
  {
    Entity i;
    i.setName(il.at(loop));
    if (db.get(i))
      continue;
    
    Data command, file;
    i.toData(keys.indexToString(KeyIndicatorDataBase::_COMMAND), command);
    i.toData(keys.indexToString(KeyIndicatorDataBase::_FILE), file);

    // launch indicator
    g_sidePanel->scriptPanel()->runScript(command.toString(), file.toString());
  }

  deleteLater();

  return 0;
}
