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

#include "PluginFactory.h"
#include "COLOR.h"
#include "COMPARE.h"
#include "MATH1.h"
#include "RANGE.h"
#include "REF.h"

#include <QDir>
#include <QDebug>
#include <QFileInfo>

PluginFactory::PluginFactory ()
{
}

PluginFactory::~PluginFactory ()
{
  qDeleteAll(indicatorPlugins);
  qDeleteAll(dbPlugins);
  qDeleteAll(libs);
}

void PluginFactory::getPluginList (QString &path, QStringList &list)
{
  list.clear();
  
  QDir dir(path);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    QFileInfo fi(QString(dir.absolutePath() + "/" + dir[loop]));
    QString s = fi.baseName();
    s.remove(0, 3);
    list.append(s);
  }

  list.sort();
}

IndicatorPlugin * PluginFactory::getIndicator (QString &path, QString &indicator)
{
  IndicatorPlugin *plug = getNotIndicatorPlugin(indicator);
  if (plug)
    return plug;
  
  plug = indicatorPlugins.value(indicator);
  if (plug)
    return plug;

  QString file = path;
  file.append("/lib" + indicator);

  QLibrary *lib = new QLibrary(file);
  IndicatorPlugin *(*so)() = 0;
  so = (IndicatorPlugin *(*)()) lib->resolve("createIndicatorPlugin");
  if (so)
  {
    plug = (*so)();
    libs.insert(indicator, lib);
    indicatorPlugins.insert(indicator, plug);
  }
  else
    delete lib;
  
  return plug;
}

DBPlugin * PluginFactory::getDB (QString &path, QString &db)
{
  DBPlugin *plug = dbPlugins.value(db);
  if (plug)
    return plug;

  QString file = path;
  file.append("/lib" + db);

  QLibrary *lib = new QLibrary(file);
  DBPlugin *(*so)() = 0;
  so = (DBPlugin *(*)()) lib->resolve("createDBPlugin");
  if (so)
  {
    plug = (*so)();
    libs.insert(db, lib);
    dbPlugins.insert(db, plug);
  }
  else
    delete lib;

  return plug;
}

IndicatorPlugin * PluginFactory::getNotIndicatorPlugin (QString &indicator)
{
  QStringList l;
  l << "COLOR" << "COMPARE" << "MATH1" << "RANGE" << "REF";

  IndicatorPlugin *plug = 0;
  int i = l.indexOf(indicator);
  switch (i)
  {
    case 0: // COLOR
      plug = new COLOR;
      break;
    case 1: // COMPARE
      plug = new COMPARE;
      break;
    case 2: // MATH1
      plug = new MATH1;
      break;
    case 3: // RANGE
      plug = new RANGE;
      break;
    case 4: // REF
      plug = new REF;
      break;
    default:
      break;
  }

  return plug;
}

