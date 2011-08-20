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
#include "Globals.h"
#include "Script.h"
#include "IPCMessage.h"
#include "SettingString.h"

#include <QtDebug>
#include <QSettings>
#include <QtDBus>

CommandScriptStart::CommandScriptStart (QObject *p) : Command (p)
{
  _type = "SCRIPT_START";
}

int CommandScriptStart::runScript (void *d)
{
  Script *script = (Script *) d;

  SettingGroup *sg = script->settingGroup(script->currentStep());
  if (! sg)
    return _ERROR;

  QStringList data;
  data << sg->get("SCRIPT")->getString();

  IPCMessage ipcm(script->session(), _type, "*", script->file());

  QDBusMessage msg = QDBusMessage::createSignal("/", "com.qtstalker.message", "message");
  msg << ipcm.toString() << data.join(",");
  QDBusConnection::sessionBus().send(msg);

  return _OK;
}

int CommandScriptStart::message (IPCMessage &, QString &d)
{
  if (d.isEmpty())
    return _ERROR;

  g_scriptPanel->runScript(d);

  return _OK;
}

SettingGroup * CommandScriptStart::settings ()
{
  SettingGroup *sg = new SettingGroup;
  sg->setCommand(_type);

  SettingString *ss = new SettingString(Setting::_NONE, Setting::_NONE, QString());
  ss->setKey("SCRIPT");
  sg->set(ss);

  return sg;
}
