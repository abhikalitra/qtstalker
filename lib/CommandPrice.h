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

#ifndef COMMAND_PRICE_HPP
#define COMMAND_PRICE_HPP

#include "Command.h"

class CommandPrice : public Command
{
  public:
    CommandPrice ();
    QString run (CommandParse &, void *);
    int getAP (void *);
    int getAP2 (Entity &, Entity &, Entity &, Entity &, Entity &line);
    int getTP (void *);
    int getTP2 (Entity &, Entity &, Entity &, Entity &line);
    int getWC (void *);
    int getWC2 (Entity &, Entity &, Entity &, Entity &line);
    
  private:
    QStringList _method;
};

#endif
