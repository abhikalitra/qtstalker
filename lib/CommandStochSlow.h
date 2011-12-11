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

#ifndef COMMAND_STOCH_SLOW_HPP
#define COMMAND_STOCH_SLOW_HPP

#include "Command.h"

class CommandStochSlow : public Command
{
  Q_OBJECT

  public:
    enum ParmType
    {
      _ParmTypeK,
      _ParmTypeD,
      _ParmTypeHigh,
      _ParmTypeLow,
      _ParmTypeClose,
      _ParmTypePeriodFastK,
      _ParmTypePeriodSlowK,
      _ParmTypePeriodSlowD,
      _ParmTypeMAK,
      _ParmTypeMAD
    };

    CommandStochSlow (QObject *);
    void runScript (CommandParse, Script *);
    QList<Data *> getSTOCHS (QList<Data *> &, int fkperiod, int skperiod, int sdperiod, int kma, int dma);
};

#endif
