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

#include "YAHOO_DIALOG.h"
//#include "YahooDialog.h"

#include <QtDebug>
#include <QDialog>

YAHOO_DIALOG::YAHOO_DIALOG ()
{
  _plugin = "YAHOO_DIALOG";
  _dialog = 0;
  _type = _DIALOG;
}

int YAHOO_DIALOG::command (Command *command)
{
  if (_dialog)
    return 0;
  
  _dialog = new YahooDialog(command);
  connect(_dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  _dialog->show();

  return 0;
}

void YAHOO_DIALOG::kill ()
{
  if (_dialog)
    _dialog->reject();
qDebug() << "YAHOO_DIALOG::kill";
}

//*************************************************************
//*************************************************************
//*************************************************************

ScriptPlugin * createScriptPlugin ()
{
  YAHOO_DIALOG *o = new YAHOO_DIALOG;
  return ((ScriptPlugin *) o);
}
