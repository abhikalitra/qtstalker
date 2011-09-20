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

#ifndef COMMAND_CSV_HPP
#define COMMAND_CSV_HPP

#include "Command.h"

class CommandCSV : public Command
{
  Q_OBJECT

  public:
    enum FormatType
    {
      _EXCHANGE,
      _SYMBOL,
      _NAME,
      _DATE,
      _OPEN,
      _HIGH,
      _LOW,
      _CLOSE,
      _VOLUME,
      _OI
    };

    enum DelimiterType
    {
      _COMMA,
      _SEMICOLON
    };

    CommandCSV (QObject *);
    int runScript (Message *, Script *);

  private:
    QStringList _formatType;
    QStringList _delimiterType;
};

#endif
