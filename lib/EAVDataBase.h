/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2011 Stefan S. Stratigakos
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

// *************************************************************************************************
// *************************************************************************************************


#ifndef EAV_DATA_BASE_HPP
#define EAV_DATA_BASE_HPP

#include <QHash>

#include "DataBase.h"
#include "Entity.h"
#include "EAVSearch.h"

class EAVDataBase : public DataBase
{
  public:
    EAVDataBase ();
    EAVDataBase (QString);
    void init ();
    int remove (QStringList);
    int set (Entity &);
    int get (Entity &);
    int search (EAVSearch &, QStringList &);
    int names (QStringList &);
    int keys (QStringList &);
};

#endif
