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

#include "CommandSetting.h"
#include "SettingString.h"
#include "SettingInteger.h"
#include "DataFactory.h"
#include "SettingFactory.h"
#include "DataSetting.h"

#include <QtDebug>

CommandSetting::CommandSetting (QObject *p) : Command (p)
{
  _name = "SETTING";
}

int CommandSetting::runScript (Message *sg, Script *script)
{
  // verify KEY
  QString key = sg->value("KEY");
  if (key.isEmpty())
  {
    qDebug() << "CommandSetting::runScript: invalid KEY";
    emit signalResume((void *) this);
    return _ERROR;
  }

  // TAB
  SettingInteger tab(0);
  QString s = sg->value("TAB");
  if (! s.isEmpty())
  {
    if (tab.set(s))
    {
      qDebug() << "CommandSetting::runScript: invalid TAB" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }

  // verify TYPE
  SettingFactory sfac;
  s = sg->value("TYPE");
  int type = sfac.stringToType(s);
  if (type == -1)
  {
    qDebug() << "CommandSetting::runScript: invalid TYPE" << s;
    emit signalResume((void *) this);
    return _ERROR;
  }

  Setting *set = sfac.setting(type);

  s = sg->value("VALUE");
  if (! s.isEmpty())
  {
    Data *d = script->data(s);
    if (d)
    {
      Setting *tset = d->get(DataSetting::_VALUE);
      if (tset)
      {
        if (set->set(tset->toString()))
        {
          qDebug() << "CommandSetting::runScript: invalid VALUE" << s;
          delete set;
          emit signalResume((void *) this);
          return _ERROR;
        }
      }
    }
    else
    {
      if (set->set(s))
      {
        qDebug() << "CommandSetting::runScript: invalid VALUE" << s;
        delete set;
        emit signalResume((void *) this);
        return _ERROR;
      }
    }
  }

  s = sg->value("LIST");
  if (! s.isEmpty() && type == SettingFactory::_LIST)
    set->set(s.split(","));

  DataFactory fac;
  Data *d = fac.data(DataFactory::_DATA_SETTING);
  d->set(DataSetting::_VALUE, set);
  d->set(DataSetting::_TAB, new SettingInteger(tab.toInteger()));
  d->set(DataSetting::_LABEL, new SettingString(key));

  script->setData(key, d);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
