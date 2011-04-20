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

#include "CUS.h"
#include "Globals.h"
#include "CUSDialog.h"
#include "Script.h"

#include <QtDebug>
#include <QEventLoop>

CUS::CUS ()
{
  _plugin = "CUS";
  _type = "INDICATOR";
}

int CUS::command (Command *command)
{
  // PARMS
  // SCRIPT
  // COMMAND

  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  BarData *bd = command->barData();
  if (! bd)
  {
    qDebug() << _plugin << "::command: no bardata";
    return 1;
  }

  QString scr = command->parm("SCRIPT");
  if (scr.isEmpty())
  {
    qDebug() << _plugin << "::command: SCRIPT missing";
    return 1;
  }

  QString com = command->parm("COMMAND");
  if (com.isEmpty())
  {
    qDebug() << _plugin << "::command: COMMAND missing";
    return 1;
  }

  Script *script = new Script(this);
  script->setIndicator(i);
  script->setBarData(bd);
  script->setName(scr);
  script->setFile(scr);
  script->setCommand(com);
  script->startScript();

  QEventLoop e;
  connect(script, SIGNAL(signalDone(QString)), &e, SLOT(quit()));
  e.exec();

  delete script;

  command->setReturnCode("0");

  return 0;
}

/*
int CUS::calculate (BarData *bd, Indicator *i, Setting *settings)
{
  Script *script = new Script(this);
  script->setIndicator(i);
  script->setBarData(bd);
  script->setName(settings->data("SCRIPT"));
  script->setFile(settings->data("SCRIPT"));
  script->setCommand(settings->data("COMMAND"));
  script->startScript();

  QEventLoop e;
  connect(script, SIGNAL(signalDone(QString)), &e, SLOT(quit()));
  e.exec();

  delete script;

  return 0;
}
*/

PluginWidget * CUS::dialog (QWidget *p)
{
  return new CUSDialog(p);
}

void CUS::defaults (QString &d)
{
  QStringList l;
  l << "PLUGIN=" + _plugin;
  l << "COMMAND=perl";
  l << "SCRIPT=";
  d = l.join(",");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CUS *o = new CUS;
  return ((Plugin *) o);
}
