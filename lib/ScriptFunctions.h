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

#ifndef SCRIPT_FUNCTIONS_HPP
#define SCRIPT_FUNCTIONS_HPP

#include <QStringList>

#include "Entity.h"
#include "EAVDataBase.h"

class ScriptFunctions
{
  public:
    ScriptFunctions ();
    int save (Entity &);
    int remove (QStringList);
    int names (QStringList &);
    int startupNames (QStringList &);
    int intervalNames (QStringList &);
    int intervalPendingNames (QStringList &);
    int load (Entity &);
    
  private:
    EAVDataBase _db;
};

#endif
