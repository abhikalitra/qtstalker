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

#include "EXCHANGE_SEARCH_DIALOG.h"
#include "ExchangeSearchDialog.h"

#include <QtDebug>
#include <QDialog>

EXCHANGE_SEARCH_DIALOG::EXCHANGE_SEARCH_DIALOG ()
{
  _plugin = "EXCHANGE_SEARCH_DIALOG";
  _type = _DIALOG;
}

int EXCHANGE_SEARCH_DIALOG::command (Command *command)
{
  // PARMS:

  ExchangeSearchDialog *dialog = new ExchangeSearchDialog(_parent, command);
  connect(dialog, SIGNAL(finished(int)), this, SIGNAL(signalResume()));
  connect(this, SIGNAL(signalKill()), dialog, SLOT(reject()));
  dialog->show();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  EXCHANGE_SEARCH_DIALOG *o = new EXCHANGE_SEARCH_DIALOG;
  return ((Plugin *) o);
}
