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

#include "CommandDialog.h"
#include "KeyDialog.h"
#include "Script.h"
#include "TypeThreadMessage.h"
#include "ThreadMessageFunctions.h"
#include "TypeMA.h"

#include <QtDebug>
#include <QColor>
#include <QUuid>

CommandDialog::CommandDialog ()
{
  _name = "DIALOG";

  KeyDialog keys;

  // string
  Data td;
  Entity::set(keys.indexToString(KeyDialog::_STRING_0), td);
  Entity::set(keys.indexToString(KeyDialog::_STRING_1), td);
  Entity::set(keys.indexToString(KeyDialog::_STRING_2), td);

  // integer
  td = Data(0);
  Entity::set(keys.indexToString(KeyDialog::_INTEGER_0), td);
  Entity::set(keys.indexToString(KeyDialog::_INTEGER_1), td);
  Entity::set(keys.indexToString(KeyDialog::_INTEGER_2), td);

  // double
  td = Data(0.0);
  Entity::set(keys.indexToString(KeyDialog::_DOUBLE_0), td);
  Entity::set(keys.indexToString(KeyDialog::_DOUBLE_1), td);
  Entity::set(keys.indexToString(KeyDialog::_DOUBLE_2), td);

  // color
  td = Data(QColor(Qt::red));
  Entity::set(keys.indexToString(KeyDialog::_COLOR_0), td);
  Entity::set(keys.indexToString(KeyDialog::_COLOR_1), td);
  Entity::set(keys.indexToString(KeyDialog::_COLOR_2), td);
  Entity::set(keys.indexToString(KeyDialog::_COLOR_3), td);

  // bool
  td = Data(FALSE);
  Entity::set(keys.indexToString(KeyDialog::_BOOL_0), td);
  Entity::set(keys.indexToString(KeyDialog::_BOOL_1), td);
  Entity::set(keys.indexToString(KeyDialog::_BOOL_2), td);

  // file
  td = Data(TypeData::_FILE);
  Entity::set(keys.indexToString(KeyDialog::_FILE_0), td);
  Entity::set(keys.indexToString(KeyDialog::_FILE_1), td);
  Entity::set(keys.indexToString(KeyDialog::_FILE_2), td);

  // ma
  TypeMA tma;
  td = Data(tma.list(), QString("EMA"));
  Entity::set(keys.indexToString(KeyDialog::_MA_0), td);
  Entity::set(keys.indexToString(KeyDialog::_MA_1), td);
  Entity::set(keys.indexToString(KeyDialog::_MA_2), td);
}

QString CommandDialog::run (CommandParse &, void *scr)
{
  Script *script = (Script *) scr;
  
  Entity dialog;
  dialog.set(QString("MESSAGE"), Data(TypeThreadMessage::_DIALOG));
  
  QList<QString> keys;
  Entity::dkeys(keys);
  
  int loop = 0;
  for (; loop < keys.size(); loop++)
  {
    Data td;
    Entity::toData(keys.at(loop), td);
    dialog.set(keys.at(loop), td);
  }
    
  QString id = QUuid::createUuid().toString();
  
  ThreadMessageFunctions tmf;
  if (tmf.sendReturn(id, dialog, script))
    return _returnCode;
  
  for (loop = 0; loop < keys.size(); loop++)
  {
    Data td;
    dialog.toData(keys.at(loop), td);
    Entity::set(keys.at(loop), td);
  }
  
  _returnCode = "OK";
  return _returnCode;
}
