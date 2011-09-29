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
#include "SettingString.h"
#include "SettingBool.h"
#include "SettingInteger.h"

#include <QtDebug>

CommandChart::CommandChart (QObject *p) : Command (p)
{
  _name = "CHART";
}

int CommandChart::runScript (Message *sg, Script *script)
{
  Data *d = new Data;
  SettingString *name = new SettingString(QString("Chart"));
  QString s = sg->value("NAME");
  if (name->set(s, (void *) script))
  {
    if (name->set(s))
    {
      qDebug() <<  "CommandChart::runScript: invalid NAME" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }
  d->set("NAME", name);

  SettingBool *date = new SettingBool(TRUE);
  s = sg->value("DATE");
  if (date->set(s, (void *) script))
  {
    if (date->set(s))
    {
      qDebug() << "CommandChart::runScript: invalid DATE" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }
  d->set("DATE", date);

  SettingBool *log = new SettingBool(FALSE);
  s = sg->value("LOG");
  if (log->set(s, (void *) script))
  {
    if (log->set(s))
    {
      qDebug() << "CommandChart::runScript: invalid LOG" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }
  d->set("LOG", log);

  SettingInteger *row = new SettingInteger(0);
  s = sg->value("ROW");
  if (row->set(s, (void *) script))
  {
    if (row->set(s))
    {
      qDebug() << "CommandChart::runScript: invalid ROW" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }
  d->set("ROW", row);

  SettingInteger *col = new SettingInteger(0);
  s = sg->value("COL");
  if (col->set(s, (void *) script))
  {
    if (col->set(s))
    {
      qDebug() << "CommandChart::runScript: invalid COL" << s;
      emit signalResume((void *) this);
      return _ERROR;
    }
  }
  d->set("COL", col);

  d->setCommand(_name);
  d->setScriptFile(script->file());
  emit signalMessage(d);

  _returnString = "OK";

  emit signalResume((void *) this);

  return _OK;
}
