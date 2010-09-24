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

#include "Yahoo.h"

Yahoo::Yahoo ()
{
  _name = "Yahoo";
  _dialog = 0;
}

Yahoo::~Yahoo ()
{
  if (_dialog)
    delete _dialog;
}

int Yahoo::configureDialog ()
{
  if (! _dialog)
  {
    _dialog = new YahooDialog;
    connect(_dialog, SIGNAL(signalChartRefresh()), this, SIGNAL(signalChartRefresh()));
  }

  _dialog->show();
  _dialog->raise();
  _dialog->activateWindow();

  return 0;
}

//**********************************************************
//**********************************************************
//**********************************************************

MiscPlugin * createMiscPlugin ()
{
  Yahoo *o = new Yahoo;
  return ((MiscPlugin *) o);
}



