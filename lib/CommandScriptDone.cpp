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

#include "CommandScriptDone.h"
#include "Globals.h"
#include "Script.h"
#include "IPCMessage.h"
#include "SettingString.h"
#include "MessageSend.h"

#include <QtDebug>
#include <QSettings>

CommandScriptDone::CommandScriptDone (QObject *p) : Command (p)
{
  _type = "SCRIPT_DONE";
}

int CommandScriptDone::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  IPCMessage ipcm(script->session(), _type, "*", script->file());

  MessageSend ms(this);
  ms.send(ipcm, sg->get("SCRIPT")->getString());

  return _OK;
}

int CommandScriptDone::message (IPCMessage &, QString &d)
{
  if (d.isEmpty())
    return _ERROR;

  g_scriptPanel->done(d);

  return _OK;
}

SettingGroup * CommandScriptDone::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_NONE, QString());
  ss->setKey("SCRIPT");
  sg->set(ss);

  return sg;
}
