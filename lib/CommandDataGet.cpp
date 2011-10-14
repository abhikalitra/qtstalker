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

#include "CommandDataGet.h"
#include "DataFactory.h"

#include <QtDebug>

CommandDataGet::CommandDataGet (QObject *p) : Command (p)
{
  _name = "DATA_GET";
}

int CommandDataGet::runScript (Message *sg, Script *script)
{
  // KEY
  QString key = sg->value("KEY");
  if (key.isEmpty())
  {
    qDebug() << "CommandDataGet::runScript: invalid KEY" << key;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Data *d = script->data(key);
  if (! d)
  {
    qDebug() << "CommandDataGet::runScript: invalid KEY" << key;
    emit signalResume((void *) this);
    return _ERROR;
  }

  switch ((DataFactory::Type) d->type())
  {
    case DataFactory::_BAR_LENGTH:
    case DataFactory::_BOOL:
    case DataFactory::_COLOR:
    case DataFactory::_CURVE:
    case DataFactory::_CURVE_BAR:
    case DataFactory::_DATE_RANGE:
    case DataFactory::_DATETIME:
    case DataFactory::_DOUBLE:
    case DataFactory::_FILE:
    case DataFactory::_INTEGER:
    case DataFactory::_LIST:
    case DataFactory::_STRING:
    case DataFactory::_SYMBOL:
      _returnString = d->toString();
      break;
    default:
      break;
  }

  emit signalResume((void *) this);

  return _OK;
}
