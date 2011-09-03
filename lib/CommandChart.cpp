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

#include "CommandChart.h"
#include "IPCMessage.h"
#include "MessageSend.h"

#include <QtDebug>

CommandChart::CommandChart (QObject *p) : Command (p)
{
  _type = "CHART";
}

int CommandChart::runScript (Data *sg, Script *script)
{
  IPCMessage ipcm(script->session(), _type, "*", script->file(), sg->type());

  MessageSend ms(this);
  ms.send(ipcm, sg->toString());

  return _OK;
}

Data * CommandChart::settings ()
{
  Data *sg = new Data;
  sg->set("NAME", QVariant(QString()));
  sg->set("DATE", QVariant(TRUE));
  sg->set("LOG", QVariant(FALSE));
  sg->set("ROW", QVariant(0));
  sg->set("COL", QVariant(0));
  return sg;
}
