/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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

#include "Qtstalker.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>

Qtstalker::Qtstalker ()
{
  pluginName = "Qtstalker";
  version = 0.2;
  createFlag = FALSE;

  set(tr("Input"), "", Setting::FileList);

  about = "Imports ascii files that were exported\n";
  about.append("previously by qtstalker's export facility.");
}

Qtstalker::~Qtstalker ()
{
}

void Qtstalker::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void Qtstalker::parse ()
{
  QStringList list = getList(tr("Input"));

  int loop;
  for (loop = 0; loop < (int) list.count(); loop++)
  {
    QFile f(list[loop]);
    if (! f.open(IO_ReadOnly))
      continue;
    QTextStream stream(&f);

    QStringList l = QStringList::split("/", list[loop], FALSE);
    QString symbol = l[l.count() - 1];

    QString s = dataPath;
    s.append("/Import");
    QDir dir(s);
    if (! dir.exists(s, TRUE))
    {
      if (! dir.mkdir(s, TRUE))
      {
        qDebug("Qtstalker plugin: Unable to create directory");
        return;
      }
    }
    s.append("/");
    s.append(symbol);
    dir.remove(s, TRUE);

    ChartDb *db = new ChartDb();
    db->openChart(s);

    while(stream.atEnd() == 0)
    {
      QString s = stream.readLine();
      s = s.stripWhiteSpace();

      if (! s.length())
        continue;

      QStringList l = QStringList::split("=", s, FALSE);
      if (l.count() != 2)
        continue;

      db->setData(l[0], l[1]);
    }

    f.close();
    delete db;
  }

  emit done();
}

Plugin * create ()
{
  Qtstalker *o = new Qtstalker;
  return ((Plugin *) o);
}




