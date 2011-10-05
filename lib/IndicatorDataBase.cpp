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
#include "Global.h"

#include <QtDebug>
#include <QSettings>

IndicatorDataBase::IndicatorDataBase ()
{
}

QStringList IndicatorDataBase::indicators ()
{
  QSettings settings(g_localSettings);
  return settings.value("indicators").toStringList();
}

int IndicatorDataBase::add (QString command, QString file)
{
  if (command.isEmpty() || file.isEmpty())
    return 1;

  QSettings settings(g_localSettings);

  QStringList indicators = settings.value("indicators").toStringList();
  indicators << file;
  indicators.removeDuplicates();

  QStringList key;
  key << "indicator" << "command" << file;
  settings.setValue(key.join("_"), command);

  settings.setValue("indicators", indicators);
  settings.sync();

  return 0;
}

int IndicatorDataBase::remove (QString file)
{
  if (file.isEmpty())
    return 1;

  QSettings settings(g_localSettings);

  QStringList indicators = settings.value("indicators").toStringList();
  int index = indicators.indexOf(file);
  if (index == -1)
    return 1;

  indicators.removeAt(index);

  QStringList key;
  key << "indicator" << "command" << file;
  settings.remove(key.join("_"));

  settings.setValue("indicators", indicators);
  settings.sync();

  return 0;
}

int IndicatorDataBase::indicator (QString file, QString &command)
{
  if (file.isEmpty())
    return 1;

  command.clear();

  QSettings settings(g_localSettings);

  QStringList key;
  key << "indicator" << "command" << file;
  command = settings.value(key.join("_")).toString();

  return 0;
}
