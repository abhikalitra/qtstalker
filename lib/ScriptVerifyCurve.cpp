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

#include "ScriptVerifyCurve.h"
#include "DataInteger.h"
#include "DataFactory.h"
#include "CurveData.h"

#include <QtDebug>

ScriptVerifyCurve::ScriptVerifyCurve ()
{
}

Data * ScriptVerifyCurve::toCurve (Script *script, QString key, int &offset)
{
  offset = 0;

  if (key.isEmpty())
    return 0;

  Data *d = script->data(key);
  if (d)
  {
    switch ((DataFactory::Type) d->type())
    {
      case DataFactory::_CURVE:
	return d;
	break;
      case DataFactory::_STRING:
	d = script->data(d->toString());
	return d;
	break;
      default:
	return 0;
    }
  }

  // verify if a curve offset version (ma.0 or ma.1)
  QStringList l = key.split(".");
  if (l.count() != 2)
    return 0;

  d = script->data(l.at(0));
  if (! d)
    return 0;

  if (d->type() != DataFactory::_CURVE)
    return 0;

  DataInteger di;
  if (di.set(l.at(1)))
    return 0;

  offset = di.toInteger();

  return d;
}
