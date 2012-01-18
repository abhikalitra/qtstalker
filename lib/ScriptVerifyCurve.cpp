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
#include "EntityType.h"
#include "DataType.h"

#include <QtDebug>

ScriptVerifyCurve::ScriptVerifyCurve ()
{
}

int ScriptVerifyCurve::curve (Script *script, QString d, Entity &e)
{
  if (script->data(d, e))
    return 1;
  
  if (e.type() != EntityType::_CURVE)
    return 1;
    
  return 0;
}

int ScriptVerifyCurve::entity (QString d, Entity &e)
{
  Data tdd(DataType::_DOUBLE);
  if (tdd.set(d, DataType::_DOUBLE))
  {
    Data tdc(DataType::_COLOR);
    if (tdc.set(d, DataType::_COLOR))
    {
      qDebug() << "ScriptVerifyCurve::entity: invalid data" << d;
      return 1;
    }
    
    e.set(QString("0"), tdc);
  }
  else
    e.set(QString("0"), tdd);
    
  return 0;
}
