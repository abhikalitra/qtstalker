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

#include "AlertItem.h"

#include <QObject>

AlertItem::AlertItem ()
{
  _statusList << QObject::tr("Waiting") << QObject::tr("Notify") << QObject::tr("Hit");
  
  _mail = 0;
  _sound = 0;
  _popup = 0;
  _id = -1;
  _status = AlertItem::_Waiting;
  _barLength = 0;
  _dateRange = 0;
  _bars = 0;
}

void AlertItem::setExchange (QString d)
{
  _exchange = d;
}

QString & AlertItem::exchange ()
{
  return _exchange;
}

void AlertItem::setSymbol (QString d)
{
  _symbol = d;
}

QString & AlertItem::symbol ()
{
  return _symbol;
}

void AlertItem::setSettings (QString d)
{
  _settings = d;
}

QString & AlertItem::settings ()
{
  return _settings;
}

void AlertItem::setIndicator (QString d)
{
  _indicator = d;
}

QString & AlertItem::indicator ()
{
  return _indicator;
}

void AlertItem::setMail (int d)
{
  _mail = d;
}

int AlertItem::mail ()
{
  return _mail;
}

void AlertItem::setSound (int d)
{
  _sound = d;
}

int AlertItem::sound ()
{
  return _sound;
}

void AlertItem::setPopup (int d)
{
  _popup = d;
}

int AlertItem::popup ()
{
  return _popup;
}

void AlertItem::setEnable (QString k, int d)
{
  _enable.insert(k, d);
}

int AlertItem::enable (QString k)
{
  return _enable.value(k);
}

QString AlertItem::enableString ()
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

void AlertItem::setEnableString (QString d)
{
  _enable.clear();
  
  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _enable.insert(l.at(loop), l.at(loop + 1).toInt());
}

void AlertItem::setOp (QString k, int d)
{
  _op.insert(k, d);
}

int AlertItem::op (QString k)
{
  return _op.value(k);
}

QString AlertItem::opString ()
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

void AlertItem::setOpString (QString d)
{
  _op.clear();

  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _op.insert(l.at(loop), l.at(loop + 1).toInt());
}

void AlertItem::setValue (QString k, double d)
{
  _value.insert(k, d);
}

double AlertItem::value (QString k)
{
  return _value.value(k);
}

QString AlertItem::valueString ()
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

void AlertItem::setValueString (QString d)
{
  _value.clear();

  QStringList l = d.split(",");

  int loop = 0;
  for (; loop < l.count(); loop += 2)
    _value.insert(l.at(loop), l.at(loop + 1).toDouble());
}

void AlertItem::setId (int d)
{
  _id = d;
}

int AlertItem::id ()
{
  return _id;
}

void AlertItem::setStatus (AlertItem::Status d)
{
  _status = d;
}

AlertItem::Status AlertItem::status ()
{
  return _status;
}

QString AlertItem::statusToString (AlertItem::Status d)
{
  return _statusList.at((int) d);
}

void AlertItem::setBarLength (int d)
{
  _barLength = d;
}

int AlertItem::barLength ()
{
  return _barLength;
}

void AlertItem::setDateRange (int d)
{
  _dateRange = d;
}

int AlertItem::dateRange ()
{
  return _dateRange;
}

int AlertItem::count ()
{
  return _enable.count();
}

void AlertItem::keys (QStringList &l)
{
  l.clear();
  
  QHashIterator<QString, int> it(_enable);
  while (it.hasNext())
  {
    it.next();
    l.append(it.key());
  }
}

void AlertItem::setBars (int d)
{
  _bars = d;
}

int AlertItem::bars ()
{
  return _bars;
}

