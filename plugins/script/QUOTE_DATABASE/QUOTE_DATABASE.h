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

#ifndef PLUGIN_QUOTE_DATABASE_HPP
#define PLUGIN_QUOTE_DATABASE_HPP

#include <QHash>
#include <QStringList>

#include "Plugin.h"

class QUOTE_DATABASE : public Plugin
{
  Q_OBJECT

  public:
    QUOTE_DATABASE ();
    ~QUOTE_DATABASE ();
    int command (Command *);
    int remove (Command *);
    int dump (Command *);
    int exchange (Command *);
    int get (Command *);
    int rename (Command *);
    int search (Command *);
    int set (Command *);
    void clear ();
    void save ();

  private:
    QStringList _method;
    QHash<QString, BarData *> _symbols;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
