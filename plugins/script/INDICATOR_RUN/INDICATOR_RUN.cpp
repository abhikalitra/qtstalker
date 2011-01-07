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

#include "INDICATOR_RUN.h"

#include <QtDebug>
#include <QEventLoop>

INDICATOR_RUN::INDICATOR_RUN ()
{
}

int INDICATOR_RUN::command (Command *command)
{
  // INDICATOR_RUN,<NAME>
  //        0         1

  if (command->count() != 2)
  {
    qDebug() << "INDICATOR_RUN::command: invalid parm count" << command->count();
    return 1;
  }

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << "INDICATOR_RUN::command: no indicator";
    return 1;
  }

  int pos = 1;
  QString name = command->parm(pos);

  i->clear();
  i->setName(name);
  i->load();
  i->calculate();
  
  QEventLoop e;
  connect(i, SIGNAL(signalPlot()), &e, SLOT(quit()));
  e.exec();

  command->setReturnData("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_RUN *o = new INDICATOR_RUN;
  return ((ScriptPlugin *) o);
}