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
#include "IPCMessage.h"
#include "MessageSend.h"
#include "Data.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

Command::Command (QObject *p) : QObject (p)
{
  _isDialog = 0;
}

int Command::runScript (Data *, Script *)
{
  return 0;
}

int Command::request (Message *, Message *)
{
  return 0;
}

Data * Command::settings ()
{
  return 0;
}

QString Command::type ()
{
  return _type;
}

int Command::isDialog ()
{
  return _isDialog;
}

QString Command::returnString ()
{
  return _returnString;
}

void Command::message (QString session, QString command, QString member, QString data)
{
/*
  Data data;
  data.set("MESSAGE", d);

  IPCMessage ipcm(session, "DEBUG", "*", QString(), data.type());
  MessageSend ms(this);
  ms.send(ipcm, data.toString());
*/

  QFile f("/tmp/QtStalkerScript.log");
  if (! f.open(QIODevice::Append | QIODevice::Text))
    return;

  QTextStream out(&f);
  out << session << ":" << command << ":" << member << ":" << data << "\n";
}
