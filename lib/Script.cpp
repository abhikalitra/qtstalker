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

#include "Script.h"


Script::Script ()
{
  status = 0;
}

void Script::setName (QString &d)
{
  name = d;
}

QString & Script::getName ()
{
  return name;
}

void Script::setCommand (QString &d)
{
  command = d;
}

QString & Script::getCommand ()
{
  return command;
}

void Script::setFile (QString &d)
{
  file = d;
}

QString & Script::getFile ()
{
  return file;
}

void Script::setComment (QString &d)
{
  comment = d;
}

QString & Script::getComment ()
{
  return comment;
}

void Script::setStatus (int d)
{
  status = d;
}

int Script::getStatus ()
{
  return status;
}

void Script::setLastRun (QDateTime &d)
{
  lastRun = d;
}

QDateTime & Script::getLastRun ()
{
  return lastRun;
}

