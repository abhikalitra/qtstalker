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

#include "CommandGroupSave.h"
#include "GroupDataBase.h"
#include "VerifyDataInput.h"

#include <QtDebug>

CommandGroupSave::CommandGroupSave (QObject *p) : Command (p)
{
  _name = "GROUP_SAVE";
}

int CommandGroupSave::runScript (Message *sg, Script *script)
{
  VerifyDataInput vdi;

  // OUTPUT
  QString name;
  QString s = sg->value("NAME");
  if (vdi.toString(script, s, name))
  {
    qDebug() << "CommandGroupSave::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  // SYMBOLS
  QStringList symbols;
  s = sg->value("SYMBOLS");
  if (vdi.toSymbol(script, s, symbols))
  {
    qDebug() << "CommandGroupSave::runScript: invalid OUTPUT" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  GroupDataBase db;
  db.transaction();
  if (db.save(name, symbols))
  {
    emit signalResume((void *) this);
    return _ERROR;
  }
  db.commit();

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
