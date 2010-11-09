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

#ifndef SCGROUP_HPP
#define SCGROUP_HPP

#include "ScriptPlugin.h"

class GROUP : public ScriptPlugin
{
  public:
    enum Method
    {
      _ADD,  // add a group item to a group
      _DELETE,  // deletes a group and contents
      _GET  // returns a csv list of the group contents
    };
    
    GROUP ();
    int command (QStringList &, Indicator &, BarData &, QByteArray &);
    int addGroup (QStringList &l, QByteArray &ba);
    int deleteGroup (QStringList &l, QByteArray &ba);
    int getGroup (QStringList &l, QByteArray &ba);

  protected:
    QStringList _methodList;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif