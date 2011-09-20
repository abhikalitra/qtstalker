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
  _type = -1;
}

int Setting::set (QString, void *)
{
  return 0;
}

int Setting::set (QString)
{
  return 0;
}


void Setting::set (int)
{
}

void Setting::set (double)
{
}

void Setting::set (QStringList)
{
}

void Setting::set (QColor)
{
}

void Setting::set (QFont)
{
}

void Setting::set (QDateTime)
{
}

void Setting::set (bool)
{
}

int Setting::toInteger ()
{
  return 0;
}

double Setting::toDouble ()
{
  return 0;
}

QStringList Setting::toList ()
{
  return QStringList();
}

QColor Setting::toColor ()
{
  return QColor();
}

QFont Setting::toFont ()
{
  return QFont();
}

QDateTime Setting::toDateTime ()
{
  return QDateTime();
}

bool Setting::toBool ()
{
  return FALSE;
}

QString Setting::toString ()
{
  return QString();
}

int Setting::type ()
{
  return _type;
}
