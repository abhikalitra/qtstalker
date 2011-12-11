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

#ifndef COMMAND_CANDLE_PATTERN_HPP
#define COMMAND_CANDLE_PATTERN_HPP

#include "Command.h"

class CommandCandlePattern : public Command
{
  Q_OBJECT

  public:
    enum ParmType
    {
      _ParmTypeOutput,
      _ParmTypeOpen,
      _ParmTypeHigh,
      _ParmTypeLow,
      _ParmTypeClose,
      _ParmTypePenetration,
      _ParmTypeMethod
    };

    CommandCandlePattern (QObject *);
    void runScript (CommandParse, Script *);
    Data * getPattern (QList<Data *> &, int type, double pen);
    Data * getCandles (QList<Data *> &, int type);
    Data * getCandlesPen (QList<Data *> &, int type, double pen);
};

#endif
