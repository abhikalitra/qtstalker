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

#include "BaseConfig.h"

#include <QtDebug>
#include <QtSql>
#include <QDir>

BaseConfig::BaseConfig ()
{
  _dbName = "config";
  _tableName = "config";
}

void BaseConfig::getData (QString &k, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "SELECT setting FROM " + _tableName + " WHERE key='" + k + "'";
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "BaseConfig::getData: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

void BaseConfig::setData (QString &k, QString &d)
{
  QSqlQuery q(QSqlDatabase::database(_dbName));
  QString s = "INSERT OR REPLACE INTO " + _tableName + " (key,setting) VALUES (";
  s.append("'" + k + "'"); // key
  s.append(",'" + d + "'"); // setting
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "BaseConfig::setData: " << q.lastError().text() << k << d;
    return;
  }
}

void BaseConfig::getData (int p, QString &d)
{
  QString s = QString::number(p);
  getData(s, d);
}

void BaseConfig::setData (int p, QString &d)
{
  QString s = QString::number(p);
  setData(s, d);
}

void BaseConfig::getData (int p, QSplitter *sp)
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

void BaseConfig::setData (int p, QSplitter *sp)
{
  QStringList l;
  QList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));

  QString s(l.join(","));
  setData(p,s);
}

void BaseConfig::getData (int name, QColor &color)
{
  QString c;
  getData(name, c);
  if (c.length())
    color.setNamedColor(c);
}

void BaseConfig::setData (int name, QColor &color)
{
  QString c = color.name();
  setData(name, c);
}

void BaseConfig::getData (int name, QFont &font)
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

void BaseConfig::setData (int name, QFont &font)
{
  QString f = font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());

  setData(name, f);
}

void BaseConfig::getData (int name, QPoint &p)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void BaseConfig::setData (int name, QPoint &p)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());
  setData(name, s);
}

void BaseConfig::getData (int name, QSize &sz)
{
  QString s;
  getData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  sz.setWidth (l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void BaseConfig::setData (int name, QSize &sz)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());
  setData(name, s);
}

bool BaseConfig::getBool (int name)
{
  QString s;
  bool b = FALSE;
  getData(name, s);
  if (s.length())
    b = s.toInt();
  return b;
}

void BaseConfig::setData (int name, bool b)
{
  QString s = QString::number(b);
  setData(name, s);
}

int BaseConfig::getInt (int name)
{
  QString s;
  int i = 0;
  getData(name, s);
  if (s.length())
    i = s.toInt();
  return i;
}

void BaseConfig::setData (int name, int i)
{
  QString s = QString::number(i);
  setData(name, s);
}

void BaseConfig::setData (int name, qint64 i)
{
  QString s = QString::number(i);
  setData(name, s);
}

double BaseConfig::getDouble (int name)
{
  QString s;
  double d = 0;
  getData(name, s);
  if (s.length())
    d = s.toDouble();
  return d;
}

void BaseConfig::setData (int name, double d)
{
  QString s = QString::number(d);
  setData(name, s);
}

void BaseConfig::getData (int name, QStringList &l)
{
  l.clear();
  QString s;
  getData(name, s);
  if (s.length())
    l = s.split(",");
}

void BaseConfig::setData (int name, QStringList &l)
{
  QString s = l.join(",");
  setData(name, s);
}

void BaseConfig::getData (int name, QDateTime &dt)
{
  QString s;
  getData(name, s);
  dt = QDateTime::fromString(s, Qt::ISODate);
}

void BaseConfig::setData (int name, QDateTime &dt)
{
  if (! dt.isValid())
    return;

  QString s = dt.toString(Qt::ISODate);
  setData(name, s);
}

