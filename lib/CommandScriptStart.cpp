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

#include "CommandScriptStart.h"
#include "IPCMessage.h"
#include "MessageSend.h"
#include "SettingString.h"

#include <QtDebug>

CommandScriptStart::CommandScriptStart (QObject *p) : Command (p)
{
  _type = "SCRIPT_START";
}

int CommandScriptStart::runScript (Message *sg, Script *script)
{
  Data d;
  SettingString *name = new SettingString;
  QString s = sg->value("SCRIPT");
  if (name->set(s, (void *) script))
  {
    if (name->set(s))
    {
      _message << "invalid SCRIPT " + s;
      return _ERROR;
    }
  }
  d.set("SCRIPT", name);

  SettingString *com = new SettingString(QString("perl"));
  s = sg->value("COMMAND");
  if (com->set(s, (void *) script))
  {
    if (com->set(s))
    {
      _message << "invalid COMMAND " + s;
      return _ERROR;
    }
  }
  d.set("COMMAND", com);

  IPCMessage ipcm(script->session(), _type, "*", script->file(), QString::number(d.type()));
  MessageSend ms(this);
  ms.send(ipcm, d.toString());

  return _OK;
}
