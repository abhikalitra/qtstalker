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
  _barLength = 6;
  _dateRange = 5;
  _bars = 0;
}

void AlertItem::setSymbol (QString d)
{
  _symbol = d;
}

QString & AlertItem::symbol ()
{
  return _symbol;
}

void AlertItem::setSettings (Setting d)
{
  _settings = d;
}

Setting & AlertItem::settings ()
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

void AlertItem::setPlots (QStringList d)
{
  _plots = d;
}

QStringList & AlertItem::plots ()
{
  return _plots;
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

void AlertItem::setBars (int d)
{
  _bars = d;
}

int AlertItem::bars ()
{
  return _bars;
}

void AlertItem::setLastUpdate (QDateTime d)
{
  _lastDate = d;
}

QDateTime AlertItem::lastUpdate ()
{
  return _lastDate;
}

void AlertItem::setHitDate (QDateTime d)
{
  _hitDate = d;
}

QDateTime AlertItem::hitDate ()
{
  return _hitDate;
}

