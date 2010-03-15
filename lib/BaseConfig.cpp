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
}

void BaseConfig::createTable ()
{
  QSqlDatabase db = QSqlDatabase::database("config");
  QSqlQuery q(db);
  QString s = "CREATE TABLE IF NOT EXISTS config (";
  s.append("key INT PRIMARY KEY UNIQUE");
  s.append(", setting TEXT");
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
    qDebug() << "BaseConfig::createTable: " << q.lastError().text();
}

void BaseConfig::transaction ()
{
  QSqlDatabase db = QSqlDatabase::database("config");
  db.transaction();
}

void BaseConfig::commit ()
{
  QSqlDatabase db = QSqlDatabase::database("config");
  db.commit();
}

void BaseConfig::getBaseData (int p, QString &d)
{
  d.clear();
  QSqlQuery q(QSqlDatabase::database("config"));
  QString s = "SELECT setting FROM config WHERE key=" + QString::number(p);
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "BaseConfig::getBaseData: " << q.lastError().text();
    return;
  }

  if (q.next())
    d = q.value(0).toString();
}

void BaseConfig::setBaseData (int p, QString &d)
{
  QSqlQuery q(QSqlDatabase::database("config"));
  QString s = "INSERT OR REPLACE INTO config (key,setting) VALUES (";
  s.append(QString::number(p)); // key
  s.append(",'" + d + "'"); // setting
  s.append(")");
  q.exec(s);
  if (q.lastError().isValid())
  {
    qDebug() << "BaseConfig::setBaseData: " << q.lastError().text() << p << " " << d;
    return;
  }
}

void BaseConfig::getBaseData (int p, QSplitter *sp)
{
  QString s;
  getBaseData(p, s);

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

void BaseConfig::setBaseData (int p, QSplitter *sp)
{
  QStringList l;
  QList<int> sizeList = sp->sizes();

  int loop;
  for (loop = 0; loop < (int) sizeList.count(); loop++)
    l.append(QString::number(sizeList[loop]));

  QString s(l.join(","));
  setBaseData(p,s);
}

void BaseConfig::getBaseData (int name, QColor &color)
{
  QString c;
  getBaseData(name, c);
  if (c.length())
    color.setNamedColor(c);
}

void BaseConfig::setBaseData (int name, QColor &color)
{
  QString c = color.name();
  setBaseData(name, c);
}

void BaseConfig::getBaseData (int name, QFont &font)
{
  QString f;
  getBaseData(name, f);
  if (! f.length())
    return;

  QStringList l = f.split(",");

  font.setFamily(l[0]);
  font.setPointSize(l[1].toInt());
  font.setWeight(l[2].toInt());
  font.setItalic(l[3].toInt());
  font.setBold(l[4].toInt());
}

void BaseConfig::setBaseData (int name, QFont &font)
{
  QString f = font.family() + "," +
              QString::number(font.pointSize()) + "," +
              QString::number(font.weight()) + "," +
              QString::number(font.italic ()) + "," +
              QString::number(font.bold ());

  setBaseData(name, f);
}

void BaseConfig::getBaseData (int name, QPoint &p)
{
  QString s;
  getBaseData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  p.setX(l[0].toInt());
  p.setY(l[1].toInt());
}

void BaseConfig::setBaseData (int name, QPoint &p)
{
  QString s = QString::number(p.x()) + "," + QString::number(p.y());
  setBaseData(name, s);
}

void BaseConfig::getBaseData (int name, QSize &sz)
{
  QString s;
  getBaseData(name, s);
  if (! s.length())
    return;

  QStringList l = s.split(",");
  sz.setWidth (l[0].toInt());
  sz.setHeight(l[1].toInt());
}

void BaseConfig::setBaseData (int name, QSize &sz)
{
  QString s = QString::number(sz.width()) + "," + QString::number(sz.height());
  setBaseData(name, s);
}

bool BaseConfig::getBool (int name)
{
  QString s;
  bool b = FALSE;
  getBaseData(name, s);
  if (s.length())
    b = s.toInt();
  return b;
}

void BaseConfig::setBaseData (int name, bool b)
{
  QString s = QString::number(b);
  setBaseData(name, s);
}

int BaseConfig::getInt (int name)
{
  QString s;
  int i = 0;
  getBaseData(name, s);
  if (s.length())
    i = s.toInt();
  return i;
}

void BaseConfig::setBaseData (int name, int i)
{
  QString s = QString::number(i);
  setBaseData(name, s);
}

double BaseConfig::getDouble (int name)
{
  QString s;
  double d = 0;
  getBaseData(name, s);
  if (s.length())
    d = s.toDouble();
  return d;
}

void BaseConfig::setBaseData (int name, double d)
{
  QString s = QString::number(d);
  setBaseData(name, s);
}

void BaseConfig::getBaseData (int name, QStringList &l)
{
  l.clear();
  QString s;
  getBaseData(name, s);
  if (s.length())
    l = s.split(",");
}

void BaseConfig::setBaseData (int name, QStringList &l)
{
  QString s = l.join(",");
  setBaseData(name, s);
}


