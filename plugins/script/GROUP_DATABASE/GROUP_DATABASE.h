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

#ifndef GROUP_DATABASE_HPP
#define GROUP_DATABASE_HPP

#include <QtSql>

#include "ScriptPlugin.h"

class GROUP_DATABASE : public ScriptPlugin
{
  public:
    enum Method
    {
      _LOAD,
      _SAVE,
      _SAVE_ALL,
      _DELETE,
      _GROUPS
    };
    
    GROUP_DATABASE ();
    void init ();
    int command (Command *);
    int load (Command *);
    int save (Command *);
    int saveAll (Command *);
    int deleteGroup (Command *);
    int groups (Command *);
    int newGroup (QString &name, QString &table);
    int getTable (QString &name, QString &table);

  private:
    QStringList _method;
    QSqlDatabase _db;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif
