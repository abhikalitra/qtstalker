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

#include <QtDebug>

Setting::Setting ()
{
}

QString Setting::getString ()
{
  return QString();
}

void Setting::setString (QString)
{
}

QStringList Setting::getList ()
{
  return QStringList();
}

void Setting::setList (QStringList)
{
}

QColor Setting::getColor ()
{
  return QColor();
}

void Setting::setColor (QColor)
{
}

QFont Setting::getFont ()
{
  return QFont();
}

void Setting::setFont (QFont)
{
}

int Setting::getInteger ()
{
  return 0;
}

void Setting::setInteger (int)
{
}

double Setting::getDouble ()
{
  return 0;
}

void Setting::setDouble (double)
{
}

QDateTime Setting::getDateTime ()
{
  return QDateTime();
}

void Setting::setDateTime (QDateTime)
{
}

QString Setting::toString ()
{
  return QString();
}

int Setting::fromString (QString)
{
}

bool Setting::getBool ()
{
  return FALSE;
}

void Setting::setBool (bool)
{
}

int Setting::getIntegerHigh ()
{
  return 0;
}

int Setting::getIntegerLow ()
{
  return 0;
}

double Setting::getDoubleHigh ()
{
  return 0;
}

double Setting::getDoubleLow ()
{
  return 0;
}

int Setting::type ()
{
  return _type;
}

int Setting::inputType ()
{
  return _inputType;
}

int Setting::outputType ()
{
  return _outputType;
}

void Setting::setTip (QString d)
{
  _tip = d;
}

QString Setting::tip ()
{
  return _tip;
}

void Setting::setKey (QString d)
{
  _key = d;
}

QString Setting::key ()
{
  return _key;
}
