/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "Config.h"
#include <QtDebug>
#include <QtSql>
#include <QDir>


Config::Config ()
{
}

void Config::init (QString session)
{
  QString s = QDir::homePath() + "/.qtstalker/config.sqlite" + session;
  QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "config");
  db.setHostName("me");
  db.setDatabaseName(s);
  db.setUserName("qtstalker");
  db.setPassword("qtstalker");
  if (! db.open())
  {
    qDebug() << "Config::Config:" << db.lastError().text();
    return;
  }

  // create the config table
  QSqlQuery q(db);
  s = "CREATE TABLE IF NOT EXISTS config (key INT PRIMARY KEY, setting TEXT)";
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "Config::createConfigTable: " << q.lastError().text();
}

void Config::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("config");
  db.transaction();
}

void Config::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("config");
  db.commit();
}

void Config::getData (Parm p, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database("config"));
  QString s = "SELECT setting FROM config WHERE key=" + QString::number(p);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Config::getData: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

void Config::setData (Parm p, QString &d)
{
  QSqlQuery q(QSqlDatabase::database("config"));
  QString s = "INSERT OR REPLACE INTO config (key,setting) VALUES (" + QString::number(p) + ",'" + d + "')";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "Config::setData: " << q.lastError().text() << p << " " << d;
    return;
  }
}

void Config::getData (Parm p, QSplitter *sp)
{
  QString s;
  getData(p, s);

  QStringList l = s.split(",");
  QList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (loop >= sizeList.count())
      break;

    if (l[loop].toInt() < 25)
      sizeList[loop] = 25;
    else
      sizeList[loop] = l[loop].toInt();
  }

  sp->setSizes(sizeList);
}

void Config::setData (Parm p, QSplitter *sp)
{
  QStringList l;
  QList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));

  QString s(l.join(","));
  setData(p,s);
}

void Config::getData (Parm name, QColor &color)
{
  QString c;
  getData(name, c);
  if (c.length())
    color.setNamedColor(c);
}

void Config::setData (Parm name, QColor &color)
{
  QString c = color.name();
  setData(name, c);
}

void Config::getData (Parm name, QFont &font)
{
  QString f;
  getData(name, f);
  if (! f.length())
    return;

  QStringList l = f.split(",");

  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());
}

void Config::setData (Parm name, QFont &font)
{
  QString f = font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());

  setData(name, f);
}

void Config::getData (Parm name, QPoint &p)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void Config::setData (Parm name, QPoint &p)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());
  setData(name, s);
}

void Config::getData (Parm name, QSize &sz)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  sz.setWidth (l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void Config::setData (Parm name, QSize &sz)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());
  setData(name, s);
}

bool Config::getBool (Parm name)
{
  QString s;
  bool b = FALSE;
  getData(name, s);
  if (s.length())
    b = s.toInt();
  return b;
}

void Config::setData (Parm name, bool b)
{
  QString s = QString::number(b);
  setData(name, s);
}

int Config::getInt (Parm name)
{
  QString s;
  int i = 0;
  getData(name, s);
  if (s.length())
    i = s.toInt();
  return i;
}

void Config::setData (Parm name, int i)
{
  QString s = QString::number(i);
  setData(name, s);
}

double Config::getDouble (Parm name)
{
  QString s;
  double d = 0;
  getData(name, s);
  if (s.length())
    d = s.toDouble();
  return d;
}

void Config::setData (Parm name, double d)
{
  QString s = QString::number(d);
  setData(name, s);
}

void Config::getData (Parm name, QStringList &l)
{
  l.clear();
  QString s;
  getData(name, s);
  if (s.length())
    l = s.split(",");
}

void Config::setData (Parm name, QStringList &l)
{
  QString s = l.join(",");
  setData(name, s);
}


