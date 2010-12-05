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

#include "CO.h"
#include "ChartObjectSettings.h"
#include "ChartObjectFactory.h"
#include "ChartObject.h"
#include "Globals.h"
#include "Config.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

CO::CO ()
{
}

int CO::command (QStringList &l, Indicator &, BarData &, QByteArray &ba)
{
  // CO,<TYPE>,*
  // 0    1
  
  int rc = 1;
  ba.clear();
  ba.append("ERROR\n");

  if (l.count() < 2)
  {
    qDebug() << "CO::command: invalid parm count" << l.count();
    return rc;
  }

  ChartObjectFactory fac;
  ChartObject *co = fac.chartObject(l[1]);
  if (! co)
  {
    qDebug() << "CO::command: invalid type" << l.at(1);
    return rc;
  }

  if (co->CUS(l))
  {
    delete co;
    return rc;
  }

  Config config;
  QString d = QString::number(config.getInt(Config::LastChartObjectID) + 1);
  config.transaction();
  config.setData(Config::LastChartObjectID, d);
  config.commit();

  ChartObjectSettings set;
  set.id = d.toInt();

  co->setSettings(set);

  ChartObjectDataBase db;
  db.transaction();
  db.setChartObject(set);
  db.commit();

  delete co;

  ba.clear();
  ba.append("0\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CO *o = new CO;
  return ((ScriptPlugin *) o);
}

