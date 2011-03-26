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

#include "DATA_WINDOW.h"
#include "Globals.h"
#include "IndicatorDataBase.h"
#include "DataWindow.h"

#include <QtDebug>
#include <QEventLoop>

DATA_WINDOW::DATA_WINDOW ()
{
  _plugin = "DATA_WINDOW";
  _type = _DIALOG;
}

int DATA_WINDOW::command (Command *command)
{
  Indicator *i = command->indicator();
  if (! i)
  {
    qDebug() << _plugin << "::command: no indicator";
    return 1;
  }

  if (! g_barData->count())
    return 1;
  
  DataWindow *dw = new DataWindow(_parent);

  IndicatorDataBase db;
  QStringList l;
  db.indicators(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    i->clear();
    i->setName(l.at(loop));
    i->load();
    i->calculate();

//    QEventLoop e;
//    connect(i, SIGNAL(signalPlot()), &e, SLOT(quit()));
//    e.exec();

    dw->setData(i);
  }

  dw->show();
  dw->scrollToBottom();

  command->setReturnCode("0");

  emit signalResume();

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  DATA_WINDOW *o = new DATA_WINDOW;
  return ((Plugin *) o);
}
