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

#include "CommandGroupPanel.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>
#include <QUuid>

CommandGroupPanel::CommandGroupPanel ()
{
  _name = "GROUP_PANEL";
  _method << "REFRESH" << "SELECT";

  Data td(TypeData::_LIST);
  Entity::set(QString("LIST"), td);

  td = Data(_method, _method.at(0));
  Entity::set(QString("METHOD"), td);
}

QString CommandGroupPanel::run (CommandParse &, void *d)
{
  Data method;
  Entity::toData(QString("METHOD"), method);
  
  switch (_method.indexOf(method.toString()))
  {
    case 0: // REFRESH
      if (refresh(d))
        return _returnCode;
      break;
    case 1: // SELECT
      if (select(d))
        return _returnCode;
      break;
    default:
      qDebug() << "CommandGroupPanel::run: invalid method" << method.toString();
      return _returnCode;
      break;
  }
  
  _returnCode = "OK";
  return _returnCode;
}

int CommandGroupPanel::select (void *d)
{
  Script *script = (Script *) d;
  
  Data list;
  Entity::toData(QString("LIST"), list);
  
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_GROUP_PANEL_SELECT));
  e.set(QString("LIST"), list);
  
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, e, script))
    return 1;
  
  e.toData(QString("LIST"), list);
  Entity::set(QString("LIST"), list);

  return 0;
}

int CommandGroupPanel::refresh (void *d)
{
  Script *script = (Script *) d;
  
  Entity e;
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_GROUP_PANEL_REFRESH));
  
  ThreadMessageFunctions tmf;
  tmf.send(e, script);
  
  return 0;
}
