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

#include "IndicatorDataBase.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>
#include <QDir>

IndicatorDataBase::IndicatorDataBase ()
{
}

int IndicatorDataBase::load (Indicator *i)
{
  QString name = i->name();
  if (name.isEmpty())
  {
    qDebug() << "IndicatorDataBase::load: invalid name";
    return 1;
  }

  QSettings db(g_localSettings);

  QString key = "indicator_" + name + "_";

  i->settings()->parse(db.value(key + "settings").toString());
  
  return 0;
}

int IndicatorDataBase::save (Indicator *i)
{
  QString name = i->name();
  if (name.isEmpty())
  {
    qDebug() << "IndicatorDataBase::save: invalid name";
    return 1;
  }

  QSettings db(g_localSettings);

  // check if this is a new indicator and add it to master list
  QStringList l = db.value("indicators").toStringList();
  if (l.indexOf(name) == -1)
  {
    l.append(name);
    db.setValue("indicators", l);
  }

  QString key = "indicator_" + name + "_";

  QString s;
  i->settings()->string(s);
  db.setValue(key + "settings", s);
  db.sync();
  
  return 0;
}

int IndicatorDataBase::deleteIndicator (QStringList &l)
{
  QSettings db(g_localSettings);

  QStringList il = db.value("indicators").toStringList();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    il.removeAll(l.at(loop));

    QString key = "indicator_" + l.at(loop) + "_settings";
    db.remove(key);
  }

  db.setValue("indicators", il);
  db.sync();

  return 0;
}

int IndicatorDataBase::indicators (QStringList &l)
{
  l.clear();
  QSettings db(g_localSettings);
  l = db.value("indicators").toStringList();
  l.sort();

  return 0;
}

int IndicatorDataBase::plugins (QStringList &l)
{
  l.clear();
  QSettings db(g_globalSettings);
  l = db.value("indicator_plugins").toStringList();
  l.sort();

  return 0;
}
