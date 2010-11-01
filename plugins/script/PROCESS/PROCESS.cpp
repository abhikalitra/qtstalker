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

#include "PROCESS.h"

#include <QtDebug>
#include <QProcess>

PROCESS::PROCESS ()
{
}

int PROCESS::command (QStringList &l, Indicator &, BarData &, QByteArray &ba)
{
  // format = PROCESS,COMMAND
  //             0      1

  ba.clear();
  ba.append("ERROR\n");

  if (l.count() != 2)
  {
    qDebug() << "PROCESS::command: invalid parm count" << l.count();
    return 1;
  }

  int rc = QProcess::startDetached(l.at(1));
  if (! rc)
  {
    qDebug() << "PROCESS::command: error starting the process";
    return 1;
  }

  ba.clear();
  ba.append("0\n");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  PROCESS *o = new PROCESS;
  return ((ScriptPlugin *) o);
}
