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

#include <QtDebug>

Command::Command (QObject *p) : QObject (p)
{
  _widgetParent = 0;
  _type = _NORMAL;
  _returnString = "ERROR";
}

Command::~Command ()
{
  qDebug() << "Command::~Command" << _name << "deleted";
}

int Command::runScript (Message *, Script *)
{
  return 0;
}

int Command::type ()
{
  return _type;
}

QString Command::name ()
{
  return _name;
}

QString Command::returnString ()
{
  return _returnString;
}

void Command::setWidgetParent (QWidget *d)
{
  _widgetParent = d;
}
