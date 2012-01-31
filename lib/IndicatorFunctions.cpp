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

#include "IndicatorFunctions.h"
#include "GlobalParent.h"
#include "GlobalSidePanel.h"
#include "Global.h"
#include "EAVSearch.h"
#include "KeyIndicatorDataBase.h"

#include <QtDebug>
#include <QSettings>

IndicatorFunctions::IndicatorFunctions ()
{
  _db = EAVDataBase("indicators");
}

int IndicatorFunctions::add (QString command, QString file)
{
  QSettings settings(g_localSettings);
  settings.setValue("add_indicator_last_script", file);
  settings.setValue("add_indicator_last_command", command);
  settings.sync();

  // launch indicator
//  g_sidePanel->scriptPanel()->runScript(command, file);
  
  return 0;
}

int IndicatorFunctions::remove (QStringList l)
{
  _db.transaction();
  if (_db.remove(l))
  {
    qDebug() << "IndicatorFunctions::remove: database error";
    return 1;
  }
  _db.commit();

//  g_plotGroup->removePlot(_indicator);

  return 0;
}

int IndicatorFunctions::list (QStringList &l)
{
  l.clear();
  
  KeyIndicatorDataBase keys;
  EAVSearch search;
  search.append(keys.indexToString(KeyIndicatorDataBase::_SESSION), "=", g_session);
  
  if (_db.search(search, l))
    return 1;
  
  return 0;
}
