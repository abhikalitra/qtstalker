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
  _type = _INDICATOR;
}

int CUS::calculate (BarData *bd, Indicator *i)
{
  Setting *settings = i->settings();

  Script *script = new Script(this);
  script->setIndicator(i);
  script->setBarData(bd);
  script->setName(settings->data(_SCRIPT));
  script->setFile(settings->data(_SCRIPT));
  script->setCommand(settings->data(_COMMAND));
  script->startScript();

  QEventLoop e;
  connect(script, SIGNAL(signalDone(QString)), &e, SLOT(quit()));
  e.exec();

  delete script;

  return 0;
}

void CUS::dialog (QWidget *p, Indicator *i)
{
  CUSDialog *dialog = new CUSDialog(p, i->settings());
  connect(dialog, SIGNAL(accepted()), i, SLOT(dialogDone()));
  dialog->show();
}

void CUS::defaults (Setting *set)
{
  set->setData("PLUGIN", _plugin);
  set->setData(_COMMAND, "perl");
  set->setData(_SCRIPT, "");
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  CUS *o = new CUS;
  return ((Plugin *) o);
}
