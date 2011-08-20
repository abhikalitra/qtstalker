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

#include "Command.h"

#include <QDebug>

Command::Command (QObject *p) : QObject (p)
{
  _isDialog = 0;
}

int Command::runScript (void *)
{
  return 0;
}

int Command::message (IPCMessage &, QString &)
{
  return 0;
}

int Command::request (Message *, Message *)
{
  return 0;
}

SettingGroup * Command::settings ()
{
  return 0;
}

QString Command::type ()
{
  return _type;
}

/*
CommandWidget * Command::dialog (QWidget *p)
{
  CommandWidget *w = new CommandWidget(p, _settings);
  return w;
}
*/

int Command::isDialog ()
{
  return _isDialog;
}
