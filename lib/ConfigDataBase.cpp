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

#include "ConfigDataBase.h"

#include <QtDebug>
#include <QDir>

ConfigDataBase::ConfigDataBase ()
{
  _dbName = "config";
  _tableName = "config";
}

void ConfigDataBase::getData (QString &k, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT setting FROM " + _tableName + " WHERE key='" + k + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ConfigDataBase::getData: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

void ConfigDataBase::setData (QString &k, QString &d)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO " + _tableName + " (key,setting) VALUES (";
  s.append("'" + k + "'"); // key
  s.append(",'" + d + "'"); // setting
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "ConfigDataBase::setData: " << q.lastError().text() << k << d;
    return;
  }
}

void ConfigDataBase::getData (int p, QString &d)
{
  QString s = QString::number(p);
  getData(s, d);
}

void ConfigDataBase::setData (int p, QString &d)
{
  QString s = QString::number(p);
  setData(s, d);
}

void ConfigDataBase::getData (int p, QSplitter *sp)
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

void ConfigDataBase::setData (int p, QSplitter *sp)
{
  QStringList l;
  QList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));

  QString s(l.join(","));
  setData(p,s);
}

void ConfigDataBase::getData (int name, QColor &color)
{
  QString c;
  getData(name, c);
  if (c.length())
    color.setNamedColor(c);
}

void ConfigDataBase::setData (int name, QColor &color)
{
  QString c = color.name();
  setData(name, c);
}

void ConfigDataBase::getData (int name, QFont &font)
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

void ConfigDataBase::setData (int name, QFont &font)
{
  QString f = font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());

  setData(name, f);
}

void ConfigDataBase::getData (int name, QPoint &p)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void ConfigDataBase::setData (int name, QPoint &p)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());
  setData(name, s);
}

void ConfigDataBase::getData (int name, QSize &sz)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  sz.setWidth (l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void ConfigDataBase::setData (int name, QSize &sz)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());
  setData(name, s);
}

bool ConfigDataBase::getBool (int name)
{
  QString s;
  bool b = FALSE;
  getData(name, s);
  if (s.length())
    b = s.toInt();
  return b;
}

void ConfigDataBase::setData (int name, bool b)
{
  QString s = QString::number(b);
  setData(name, s);
}

int ConfigDataBase::getInt (int name)
{
  QString s;
  int i = 0;
  getData(name, s);
  if (s.length())
    i = s.toInt();
  return i;
}

void ConfigDataBase::setData (int name, int i)
{
  QString s = QString::number(i);
  setData(name, s);
}

void ConfigDataBase::setData (int name, qint64 i)
{
  QString s = QString::number(i);
  setData(name, s);
}

double ConfigDataBase::getDouble (int name)
{
  QString s;
  double d = 0;
  getData(name, s);
  if (s.length())
    d = s.toDouble();
  return d;
}

void ConfigDataBase::setData (int name, double d)
{
  QString s = QString::number(d);
  setData(name, s);
}

void ConfigDataBase::getData (int name, QStringList &l)
{
  l.clear();
  QString s;
  getData(name, s);
  if (s.length())
    l = s.split(",");
}

void ConfigDataBase::setData (int name, QStringList &l)
{
  QString s = l.join(",");
  setData(name, s);
}

void ConfigDataBase::getData (int name, QDateTime &dt)
{
  QString s;
  getData(name, s);
  dt = QDateTime::fromString(s, Qt::ISODate);
}

void ConfigDataBase::setData (int name, QDateTime &dt)
{
  if (! dt.isValid())
    return;

  QString s = dt.toString(Qt::ISODate);
  setData(name, s);
}

