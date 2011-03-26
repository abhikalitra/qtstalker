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

#include "Setting.h"

#include <QObject>

Setting::Setting ()
{
}

void Setting::setData (QString k, QString d)
{
  QString s = k;
  strip(s);
  
  QString s2 = d;
  strip(s2);
  
  dict.insert(s, s2);
}

void Setting::setData (int k, QString d)
{
  QString s = QString::number(k);
  QString s2 = d;
  strip(s2);
  dict.insert(s, s2);
}

void Setting::setData (QString k, int d)
{
  QString s = k;
  strip(s);
  
  QString s2 = QString::number(d);
  
  dict.insert(s, s2);
}

void Setting::setData (int k, int d)
{
  QString s = QString::number(k);
  QString s2 = QString::number(d);
  dict.insert(s, s2);
}

void Setting::setData (QString k, double d)
{
  QString s = k;
  strip(s);
  
  QString s2 = QString::number(d);
  
  dict.insert(s, s2);
}

void Setting::setData (int k, double d)
{
  QString s = QString::number(k);
  QString s2 = QString::number(d);
  dict.insert(s, s2);
}

void Setting::setData (QString k, QDateTime d)
{
  QString s = k;
  strip(s);

  QString s2 = d.toString(Qt::ISODate);

  dict.insert(s, s2);
}

void Setting::setData (QString k, QColor d)
{
  QString s = k;
  strip(s);

  QString s2 = d.name();

  dict.insert(s, s2);
}

void Setting::setData (QString k, QFont d)
{
  QString s = k;
  strip(s);

  QString s2 = d.toString();

  dict.insert(s, s2);
}

QString Setting::data (QString k)
{
  return dict.value(k);
}

QString Setting::data (int k)
{
  return dict.value(QString::number(k));
}

double Setting::getDouble (QString k)
{
  if (dict.contains(k))
    return dict.value(k).toDouble();
  else
    return 0;
}

double Setting::getDouble (int d)
{
  QString k = QString::number(d);
  if (dict.contains(k))
    return dict.value(k).toDouble();
  else
    return 0;
}

int Setting::getInt (QString k)
{
  if (dict.contains(k))
    return dict.value(k).toInt();
  else
    return 0;
}

int Setting::getInt (int d)
{
  QString k = QString::number(d);
  if (dict.contains(k))
    return dict.value(k).toInt();
  else
    return 0;
}

QDateTime Setting::dateTime (QString k)
{
  return QDateTime::fromString(dict.value(k), Qt::ISODate);
}

QColor Setting::color (QString k)
{
  return QColor(dict.value(k));
}

QFont Setting::font (QString k)
{
  QFont font;
  font.fromString(dict.value(k));
  return font;
}

void Setting::keyList (QStringList &l)
{
  l.clear();
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }
}

void Setting::remove (QString k)
{
  dict.remove(k);
}

void Setting::string (QString &s)
{
  s.clear();
  QStringList l;
  QHashIterator<QString, QString> it(dict);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key() + "=" + it.value());
  }
  s = l.join("|");
}

void Setting::parse (QString d)
{
  dict.clear();

  QStringList l = d.split("|", QString::SkipEmptyParts);

  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    QStringList l2 = l.at(loop).split("=", QString::SkipEmptyParts);
    if (l2.count() > 1)
      dict.insert(l2.at(0), l2.at(1));
  }
}

void Setting::clear ()
{
  dict.clear();
}

int Setting::count ()
{
  return dict.count();
}

void Setting::strip (QString &d)
{
  d = d.remove(QString("="), Qt::CaseSensitive);
  d = d.remove(QString("|"), Qt::CaseSensitive);
  d = d.remove(QString("'"), Qt::CaseSensitive);
}
