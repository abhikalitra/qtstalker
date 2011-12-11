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

#include "CommandThread.h"
#include "Script.h"
#include "Command.h"

#include <QDebug>

CommandThread::CommandThread (QObject *p, void *script, CommandParse cp, void *command) : QThread (p)
{
  _cp = cp;
  _script = script;
  _command = command;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void CommandThread::run ()
{
  Script *script = (Script *) _script;
  Command *command = (Command *) _command;
  command->runScript(_cp, script);
}
