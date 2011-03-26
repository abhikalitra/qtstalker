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

#include "SYMBOL_CURRENT.h"
#include "Globals.h"

#include <QtDebug>

SYMBOL_CURRENT::SYMBOL_CURRENT ()
{
  _plugin = "SYMBOL_CURRENT";
}

int SYMBOL_CURRENT::command (Command *command)
{
  command->setReturnData(_plugin + "_EXCHANGE", g_barData->exchange());
  command->setReturnData(_plugin + "_SYMBOL", g_barData->symbol());

  command->setReturnCode("0");

  return 0;
}

//*************************************************************
//*************************************************************
//*************************************************************

Plugin * createPlugin ()
{
  SYMBOL_CURRENT *o = new SYMBOL_CURRENT;
  return ((Plugin *) o);
}
