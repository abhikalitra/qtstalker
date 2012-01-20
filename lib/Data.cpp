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

#include "Data.h"

#include <QtDebug>

Data::Data ()
{
  clear();
}

Data::Data (TypeData::Key type)
{
  clear();
  _type = type;
}

Data::Data (QString d)
{
  clear();
  _type = TypeData::_STRING;
  set(d);
}

Data::Data (QStringList d)
{
  clear();
  _type = TypeData::_LIST;
  set(d);
}

Data::Data (QStringList l, QString d)
{
  clear();
  _type = TypeData::_LIST;
  set(d);
  set(l);
}

Data::Data (int d)
{
  clear();
  _type = TypeData::_INTEGER;
  set(d);
}

Data::Data (double d)
{
  clear();
  _type = TypeData::_DOUBLE;
  set(d);
}

Data::Data (bool d)
{
  clear();
  _type = TypeData::_BOOL;
  set(d);
}

Data::Data (QColor d)
{
  clear();
  _type = TypeData::_COLOR;
  set(d);
}

Data::Data (QFont d)
{
  clear();
  _type = TypeData::_FONT;
  set(d);
}

Data::Data (QDateTime d)
{
  clear();
  _type = TypeData::_DATETIME;
  set(d);
}

void Data::clear ()
{
  _type = TypeData::_STRING;
  _tab = 0;
}

int Data::type ()
{
  return _type;
}

void Data::setType (int d)
{
  _type = d;
}

int Data::set (QString d)
{
//  _type = TypeData::_STRING;
  _value = QVariant(d);
  return 0;
}

int Data::set (QStringList d)
{
  _list = d;
  return 0;
}

int Data::set (QStringList l, QString d)
{
  set(d);
  set(l);
  return 0;
}

int Data::set (int d)
{
//  _type = TypeData::_INTEGER;
  _value = QVariant(d);
  return 0;
}

int Data::set (double d)
{
//  _type = TypeData::_DOUBLE;
  _value = QVariant(d);
  return 0;
}

int Data::set (bool d)
{
//  _type = TypeData::_BOOL;
  _value = QVariant(d);
  return 0;
}

int Data::set (QColor d)
{
//  _type = TypeData::_COLOR;
  _value = QVariant(d.name());
  return 0;
}

int Data::set (QFont d)
{
//  _type = TypeData::_FONT;
  _value = QVariant(d.toString());
  return 0;
}

int Data::set (QDateTime d)
{
//  _type = TypeData::_DATETIME;
  _value = QVariant(d);
  return 0;
}

int Data::set (QString d, int type)
{
  int rc = 0;
  
  switch ((TypeData::Key) type)
  {
    case TypeData::_BOOL:
    {
      if (d == "true" || d == "TRUE" || d == "1")
	set(TRUE);
      else
      {
        if (d == "false" || d == "FALSE" || d == "0")
	  set(FALSE);
	else
	  rc++;
      }
      break;
    }
    case TypeData::_COLOR:
    {
      QColor c(d);
      if (c.isValid())
	set(c);
      else
        rc++;
      break;
    }
    case TypeData::_DATETIME:
    {
      QDateTime tdt = QDateTime::fromString(d, Qt::ISODate);
      if (tdt.isValid())
	set(tdt);
      else
        rc++;
      break;
    }
    case TypeData::_DOUBLE:
    {
      bool ok;
      double td = d.toDouble(&ok);
      if (ok)
        set(td);
      else
        rc++;
      break;
    }
    case TypeData::_FONT:
    {
      QFont f;
      if (f.fromString(d))
	set(f);
      else
        rc++;
      break;
    }
    case TypeData::_INTEGER:
    {
      bool ok;
      int ti = d.toInt(&ok);
      if (ok)
        set(ti);
      else
        rc++;
      break;
    }
    case TypeData::_LIST:
      if (! _list.size())
        set(d);
      else
      {
        if (_list.indexOf(d) != -1)
          set(d);
        else
          rc++;
      }
      break;
    case TypeData::_STRING:
      set(d);
      break;
    case TypeData::_FILE:
    {
      QStringList tl = d.split(";");
      if (tl.size())
      {
        _type = TypeData::_FILE;
	_list = tl;
      }
      else
        rc++;
      break;
    }
    default:
      rc = 1;
      break;
  }
  
  return rc;
}

QString Data::toString ()
{
  QString s;
  switch ((TypeData::Key) _type)
  {
    case TypeData::_FILE:
      s = _list.join(";");
      break;
    default:
      s = _value.toString();
      break;
  }
  
  return s;
}

QStringList Data::toList ()
{
  return _list;
}

int Data::toInteger ()
{
  int d = _value.toInt();
  
  if (_type == (TypeData::_LIST))
    d = _list.indexOf(_value.toString());
  
  return d;
}

double Data::toDouble ()
{
  return _value.toDouble();
}

bool Data::toBool ()
{
  return _value.toBool();
}

QColor Data::toColor ()
{
  return QColor(_value.toString());
}

QFont Data::toFont ()
{
  QFont f;
  f.fromString(_value.toString());
  return f;
}

QDateTime Data::toDateTime ()
{
  return _value.toDateTime();
}

void Data::setTab (int d)
{
  _tab = d;
}

int Data::tab ()
{
  return _tab;
}

void Data::setLabel (QString d)
{
  _label = d;
}

QString Data::label ()
{
  return _label;
}
