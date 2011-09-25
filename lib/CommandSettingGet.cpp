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

#include "CommandSettingGet.h"
#include "DataSetting.h"

#include <QtDebug>

CommandSettingGet::CommandSettingGet (QObject *p) : Command (p)
{
  _type = "SETTING_GET";
}

int CommandSettingGet::runScript (Message *sg, Script *script)
{
  // verify KEY
  QString key = sg->value("KEY");
  if (key.isEmpty())
  {
    _message << "invalid KEY";
    return _ERROR;
  }

  Data *d = script->data(key);
  if (! d)
  {
    _message << "invalid KEY " + key;
    return _ERROR;
  }

  Setting *set = d->get(DataSetting::_VALUE);
  if (! set)
  {
    _message << "invalid KEY " + key;
    return _ERROR;
  }

  _returnString = set->toString();

  return _OK;
}
