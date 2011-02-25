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

#include "CHART_OBJECT_VLINE_DIALOG.h"
#include "ChartObjectVLineDialog.h"

#include <QtDebug>
#include <QDialog>

CHART_OBJECT_VLINE_DIALOG::CHART_OBJECT_VLINE_DIALOG ()
{
  _plugin = "CHART_OBJECT_VLINE_DIALOG";
  _type = _DIALOG;
}

int CHART_OBJECT_VLINE_DIALOG::command (Command *command)
{
  // PARMS:
  // ID

  QString id = command->parm("ID");
  if (id.isEmpty())
  {
    qDebug() << _plugin << "::command: invalid parm count" << id;
    return 1;
  }

  ChartObjectVLineDialog *dialog = new ChartObjectVLineDialog(command);
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  CHART_OBJECT_VLINE_DIALOG *o = new CHART_OBJECT_VLINE_DIALOG;
  return ((ScriptPlugin *) o);
}
