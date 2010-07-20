/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#include "QSDetail.h"

#include <QDebug>

QSDetail::QSDetail ()
{
  _typeList << "Name" << "Sector" << "FuturesCode" << "FuturesExpMonth" << "FuturesExpYear";
  _typeList << "T1" << "T2" << "T3" << "T4" << "T5" << "T6" << "T7" << "T8" << "T9" << "T10";
  _typeList << "T11" << "T12" << "T13" << "T14" << "T15";
  _typeList << "D1" << "D2" << "D3" << "D4" << "D5" << "D6" << "D7" << "D8" << "D9" << "D10";
  _typeList << "D11" << "D12" << "D13" << "D14" << "D15";
}

void QSDetail::setTable (QString &d)
{
  _table = d;
}

QString & QSDetail::table ()
{
  return _table;
}

int QSDetail::addKey (QString &d)
{
  if (_typeList.indexOf(d) == -1)
    return 1;

  _keys.append(d);
  
  return 0;
}

void QSDetail::addData (QString &d)
{
  _data.append(d);
}

void QSDetail::string (QString &d)
{
  d = _data.join(",");
}

void QSDetail::keyString (QString &d)
{
  d = _keys.join(",");
}

int QSDetail::keyCount ()
{
  return _keys.count();
}

QStringList & QSDetail::keyList ()
{
  return _keys;
}

QStringList & QSDetail::dataList ()
{
  return _data;
}

int QSDetail::isNumber (QString &d)
{
  int rc = 0;

  switch (_typeList.indexOf(d))
  {
    case _FuturesExpYear:
    case _D1:
    case _D2:
    case _D3:
    case _D4:
    case _D5:
    case _D6:
    case _D7:
    case _D8:
    case _D9:
    case _D10:
    case _D11:
    case _D12:
    case _D13:
    case _D14:
    case _D15:
      rc = 1;
      break;
    default:
      break;
  }

  return rc;
}

