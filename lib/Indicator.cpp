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

#include "Indicator.h"

#include <QDebug>

Indicator::Indicator ()
{
  init();
}

void Indicator::setName (QString d)
{
  _name = d;
}

QString & Indicator::name ()
{
  return _name;
}

void Indicator::setTabRow (int d)
{
  _tabRow = d;
}

int Indicator::tabRow ()
{
  return _tabRow;
}

void Indicator::setDate (int d)
{
  _date = d;
}

int Indicator::date ()
{
  return _date;
}

void Indicator::setLog (int d)
{
  _log = d;
}

int Indicator::getLog ()
{
  return _log;
}

void Indicator::setScript (QString d)
{
  _script = d;
}

QString & Indicator::script ()
{
  return _script;
}

void Indicator::setCommand (QString d)
{
  _command = d;
}

QString & Indicator::command ()
{
  return _command;
}

void Indicator::setLine (QString k, Curve *d)
{
  deleteLine(k);
  _lines.insert(k, d);
}

void Indicator::setLine (int k, Curve *d)
{
  QString s = QString::number(k);
  setLine(s, d);
}

Curve * Indicator::line (QString k)
{
  return _lines.value(k);
}

int Indicator::deleteLine (QString k)
{
  Curve *line = _lines.value(k);
  if (! line)
    return 1;

  delete line;
  _lines.remove(k);

  return 0;
}

QHash<QString, Curve *> &  Indicator::curves ()
{
  return _lines;
}

void Indicator::clear ()
{
  init();
  deleteLines();
  clearChartObjects();
}

Setting Indicator::chartObject (int k)
{
  return _chartObjects.value(k);
}

void Indicator::addChartObject (Setting &co)
{
  _chartObjects.insert(co.getInt("ID"), co);
}

void Indicator::clearChartObjects ()
{
  _chartObjects.clear();
}

void Indicator::deleteChartObject (int d)
{
  _chartObjects.remove(d);
}

void Indicator::weedPlots ()
{
  QList<QString> keys = _lines.keys();
  int loop = 0;
  for (; loop < keys.count(); loop++)
  {
    Curve *curve = _lines.value(keys.at(loop));
    if (curve->z() < 0)
    {
      delete curve;
      _lines.remove(keys.at(loop));
    }
  }
}

void Indicator::clean ()
{
  _lines.clear();
  _chartObjects.clear();
  init();
}

void Indicator::init ()
{
  _tabRow = 1;
  _date = 1;
  _log = 0;
}

void Indicator::coKeys (QList<int> &l)
{
  l = _chartObjects.keys();
}

int Indicator::coCount ()
{
  return _chartObjects.count();
}

void Indicator::deleteLines ()
{
  if (_lines.count())
  {
    qDeleteAll(_lines);
    _lines.clear();
  }
}

void Indicator::clearLines ()
{
  _lines.clear();
}

QString Indicator::toString ()
{
  QStringList l;
  l << _command << _script << QString::number(_tabRow) << QString::number(_log) << QString::number(_date);
  
  return l.join(",");
}

int Indicator::fromString (QString d)
{
  QStringList l = d.split(",", QString::SkipEmptyParts);
  if (l.count() != 5)
    return 1;

  int pos = 0;
  _command = l.at(pos++);
  _script = l.at(pos++);
  _tabRow = l.at(pos++).toInt();
  _log = l.at(pos++).toInt();
  _date = l.at(pos++).toInt();
  
  return 0;
}
