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

#include "Splitter.h"
#include "Globals.h"

#include <QDebug>
#include <QSettings>

Splitter::Splitter (QString d)
{
  _key = d;

//  load();
}

Splitter::~Splitter ()
{
//  save();
}

void Splitter::load ()
{
  QSettings settings(g_settingsFile);

  QStringList l = settings.value(_key).toStringList();
  if (! l.count())
  {
    QList<int> l2;
    l2 << 437 << 20 << 200;
    setSizes(l2);
  }
  else
  {
    QList<int> sizeList = sizes();

    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      if (loop >= sizeList.count())
        break;

      if (l[loop].toInt() < 25)
        sizeList[loop] = 25;
      else
        sizeList[loop] = l[loop].toInt();
    }

    setSizes(sizeList);
  }
}

void Splitter::save ()
{
  QSettings settings(g_settingsFile);

  QStringList l;
  QList<int> sizeList = sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));

  settings.setValue(_key, l);
  settings.sync();
}
