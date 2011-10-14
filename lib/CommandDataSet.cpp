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

#include "CommandDataSet.h"
#include "DataFactory.h"
#include "VerifyDataInput.h"

#include <QtDebug>

CommandDataSet::CommandDataSet (QObject *p) : Command (p)
{
  _name = "DATA_SET";
}

int CommandDataSet::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // KEY
  QString key;
  QString s = sg->value("KEY");
  if (vdi.toString(script, s, key))
  {
    qDebug() << "CommandDataSet::runScript: invalid KEY";
    emit signalResume((void *) this);
    return _ERROR;
  }

  // TYPE
  DataFactory fac;
  s = sg->value("TYPE");
  int type = fac.stringToType(s);
  if (type == -1)
  {
    qDebug() << "CommandDataSet::runScript: invalid TYPE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // VALUE
  QString value;
  s = sg->value("VALUE");
  vdi.toString(script, s, value);

  // TAB
  int tab = 0;
  s = sg->value("TAB");
  if (vdi.toInteger(script, s, tab))
    tab = 0;

  int insertFlag = 0;
  Data *d = script->data(key);
  if (! d)
  {
    d = fac.data(type);
    insertFlag++;
  }

  d->setTab(tab);
  d->setLabel(key);

  // LIST
  s = sg->value("LIST");
  QStringList list = s.split(",");
  d->set(list);

  if (! value.isEmpty())
  {
    if (d->set(value))
    {
      qDebug() << "CommandDataSet::runScript: invalid VALUE" << value;
      delete d;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }

  if (insertFlag)
    script->setData(key, d);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
