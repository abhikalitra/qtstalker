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

#include "INDICATOR_NEW_DIALOG.h"
#include "IndicatorNewDialog.h"

#include <QtDebug>
#include <QDialog>

INDICATOR_NEW_DIALOG::INDICATOR_NEW_DIALOG ()
{
  _threadSafe = 0;
}

int INDICATOR_NEW_DIALOG::command (Command &)
{
  // INDICATOR_NEW_DIALOG
  //        0

  IndicatorNewDialog *dialog = new IndicatorNewDialog;
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  connect(dialog, SIGNAL(signalDone(QString)), this, SIGNAL(signalDone(QString)));
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  INDICATOR_NEW_DIALOG *o = new INDICATOR_NEW_DIALOG;
  return ((ScriptPlugin *) o);
}
