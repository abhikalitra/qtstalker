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

#include "CommandChart.h"
#include "TypeThreadMessage.h"
#include "Script.h"
#include "ThreadMessageFunctions.h"

#include <QtDebug>

CommandChart::CommandChart ()
{
  _name = "CHART";

  Data td;
  td.setLabel(QObject::tr("Chart Name"));
  Entity::set(QString("NAME"), td);
  
  td = Data(TRUE);
  td.setLabel(QObject::tr("Date Axis"));
  Entity::set(QString("DATE"), td);

  td = Data(FALSE);
  td.setLabel(QObject::tr("Log Scaling"));
  Entity::set(QString("LOG"), td);

  td = Data(0);
  td.setLabel(QObject::tr("Tab Row"));
  Entity::set(QString("ROW"), td);

  td = Data(99);
  td.setLabel(QObject::tr("Tab Column"));
  Entity::set(QString("COL"), td);
}

QString CommandChart::run (CommandParse &, void *d)
{
  Script *script = (Script *) d;
  
  Entity e;
  QList<QString> keys;
  Entity::dkeys(keys);
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data td;
    Entity::toData(keys.at(loop), td);
    e.set(keys.at(loop), td);
  }
  e.set(QString("MESSAGE"), Data(TypeThreadMessage::_CHART_NEW));
  e.set(QString("COMMAND"), Data(script->command()));
  
  Data td(TypeData::_FILE);
  QStringList tl;
  tl << script->file();
  td.set(tl);
  e.set(QString("FILE"), td);

  ThreadMessageFunctions tmf;
  tmf.send(e, script);
  
  _returnCode = "OK";
  return _returnCode;
}
