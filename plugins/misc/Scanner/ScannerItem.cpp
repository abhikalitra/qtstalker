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

#include "ScannerItem.h"

#include <QDebug>

ScannerItem::ScannerItem ()
{
  _barLength = 0;
  _dateRange = 0;
}

void ScannerItem::setName (QString d)
{
  _name = d;
}

QString & ScannerItem::name ()
{
  return _name;
}

void ScannerItem::setGroup (Group &d)
{
  _group = d;
}

Group & ScannerItem::group ()
{
  return _group;
}

void ScannerItem::setSettings (QString d)
{
  _settings = d;
}

QString & ScannerItem::settings ()
{
  return _settings;
}

void ScannerItem::setIndicator (QString d)
{
  _indicator = d;
}

QString & ScannerItem::indicator ()
{
  return _indicator;
}

void ScannerItem::setEnable (QString k, int d)
{
  _enable.insert(k, d);
}

int ScannerItem::enable (QString k)
{
  return _enable.value(k);
}

QString ScannerItem::enableString ()
{
  QStringList l;
  QHashIterator<QString, int> it(_enable);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
    l.append(QString::number(it.value()));
  }

  return l.join(",");
}

void ScannerItem::setEnableString (QString d)
{
  _enable.clear();

  if (d.isEmpty())
    return;
  
  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _enable.insert(l.at(loop), l.at(loop + 1).toInt());
}

void ScannerItem::setOp (QString k, int d)
{
  _op.insert(k, d);
}

int ScannerItem::op (QString k)
{
  return _op.value(k);
}

QString ScannerItem::opString ()
{
  QStringList l;
  QHashIterator<QString, int> it(_op);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
    l.append(QString::number(it.value()));
  }

  return l.join(",");
}

void ScannerItem::setOpString (QString d)
{
  _op.clear();

  if (d.isEmpty())
    return;

  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _op.insert(l.at(loop), l.at(loop + 1).toInt());
}

void ScannerItem::setValue (QString k, double d)
{
  _value.insert(k, d);
}

double ScannerItem::value (QString k)
{
  return _value.value(k);
}

QString ScannerItem::valueString ()
{
  QStringList l;
  QHashIterator<QString, double> it(_value);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
    l.append(QString::number(it.value()));
  }

  return l.join(",");
}

void ScannerItem::setValueString (QString d)
{
  _value.clear();

  if (d.isEmpty())
    return;

  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _value.insert(l.at(loop), l.at(loop + 1).toDouble());
}

void ScannerItem::setBarLength (int d)
{
  _barLength = d;
}

int ScannerItem::barLength ()
{
  return _barLength;
}

void ScannerItem::setDateRange (int d)
{
  _dateRange = d;
}

int ScannerItem::dateRange ()
{
  return _dateRange;
}

int ScannerItem::count ()
{
  return _enable.count();
}

void ScannerItem::keys (QStringList &l)
{
  l.clear();
  
  QHashIterator<QString, int> it(_enable);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }
}

void ScannerItem::setGroupName (QString d)
{
  _groupName = d;
}

QString & ScannerItem::groupName ()
{
  return _groupName;
}

