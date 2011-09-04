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

#include "ScriptTimer.h"
#include "Script.h"

#include <QtDebug>

ScriptTimer::ScriptTimer (QObject *p) : QTimer (p)
{
   connect(this, SIGNAL(timeout()), this, SLOT(startScript()));
}

void ScriptTimer::setIntervalString (QString d)
{
  int t = d.toInt() * 1000;
  setInterval(t);
}

void ScriptTimer::setFile (QString d)
{
  _file = d;
}

void ScriptTimer::setName (QString d)
{
  _name = d;
}

void ScriptTimer::startScript ()
{
  emit signalStartScript(_file, _command);
}

void ScriptTimer::setCommand (QString d)
{
  _command = d;
}

