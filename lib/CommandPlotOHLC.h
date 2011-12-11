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

#ifndef COMMAND_PLOT_OHLC_HPP
#define COMMAND_PLOT_OHLC_HPP

#include "Command.h"

class CommandPlotOHLC : public Command
{
  Q_OBJECT

  public:
    enum ParmType
    {
      _ParmTypeName,
      _ParmTypeChart,
      _ParmTypeLabel,
      _ParmTypeStyle,
      _ParmTypeColor,
      _ParmTypeZ,
      _ParmTypePen,
      _ParmTypeOpen,
      _ParmTypeHigh,
      _ParmTypeLow,
      _ParmTypeClose
    };

    CommandPlotOHLC (QObject *);
    void runScript (CommandParse, Script *);
};

#endif
