/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#include "QtstalkerFormat.h"
#include "ChartDb.h"
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qdir.h>

QtstalkerFormat::QtstalkerFormat ()
{
  pluginName = "QtstalkerFormat";
  createFlag = FALSE;

  set(tr("Input"), "", Setting::FileList);

  about = "Imports ascii files that were exported\n";
  about.append("previously by qtstalker's export facility.");
}

QtstalkerFormat::~QtstalkerFormat ()
{
}

void QtstalkerFormat::update ()
{
  QTimer::singleShot(250, this, SLOT(parse()));
}

void QtstalkerFormat::parse ()
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

    QString path = createDirectory("Import");
    if (! path.length())
    {
      qDebug("QtstalkerFormat plugin: Unable to create directory");
      return;
    }

    path.append("/");
    path.append(symbol);
    QDir dir(path);
    dir.remove(path, TRUE);

    ChartDb *db = new ChartDb();
    db->openChart(path);

    QString s = tr("Updating ");
    s.append(symbol);
    emit message(s);

    while(stream.atEnd() == 0)
    {
      s = stream.readLine();
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
  QtstalkerFormat *o = new QtstalkerFormat;
  return ((Plugin *) o);
}




