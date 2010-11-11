/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#include "CSV.h"

CSV::CSV ()
{
  _name = "CSV";
  _dialog = 0;
}

CSV::~CSV ()
{
  if (_dialog)
    delete _dialog;
}

int CSV::configureDialog ()
{
  if (! _dialog)
    return 0;
  
  _dialog->show();
  _dialog->raise();
  _dialog->activateWindow();
  
  return 0;
}

void CSV::initDialog ()
{
  _dialog = new CSVDialog;
  connect(_dialog, SIGNAL(signalChartRefresh()), this, SIGNAL(signalChartRefresh()));
  connect(_dialog, SIGNAL(signalMessage(QString)), this, SIGNAL(signalMessage(QString)));
  _dialog->activateWindow();
}

//**********************************************************
//**********************************************************
//**********************************************************

MiscPlugin * createMiscPlugin ()
{
  CSV *o = new CSV;
  return ((MiscPlugin *) o);
}


