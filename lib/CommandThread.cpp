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
#include "ScriptPluginFactory.h"

#include <QDebug>

CommandThread::CommandThread (QObject *p, Command com) : QThread (p)
{
  _command = com;

  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QString CommandThread::stringData ()
{
  return _command.stringData();
}

QByteArray CommandThread::arrayData ()
{
  return _command.arrayData();
}

void CommandThread::run ()
{
  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(_command.plugin());
  if (! plug)
  {
    qDebug() << "CommandThread::run: no plugin" << _command.plugin();
    return;
  }
  
  plug->command(_command);

  delete plug;

  emit signalDone(_command.stringData());
}