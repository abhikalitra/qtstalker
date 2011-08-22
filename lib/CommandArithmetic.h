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

#ifndef COMMAND_ARITHMETIC_HPP
#define COMMAND_ARITHMETIC_HPP

#include "Command.h"
#include "Curve.h"

class CommandArithmetic : public Command
{
  Q_OBJECT

  public:
    CommandArithmetic (QObject *);
    int runScript (void *);
    SettingGroup * settings ();
    Curve * getArithmetic (Curve *in, Curve *in2, int method);

  private:
    QStringList _method;
};

#endif
