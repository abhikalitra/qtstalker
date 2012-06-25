/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#ifndef PLUGIN_SCRIPT_H
#define PLUGIN_SCRIPT_H

#include <QObject>

#include "PluginIndicator.h"
#include "CommandParse.h"

class Script : public QObject, public PluginIndicator
{
  Q_OBJECT
  Q_INTERFACES(PluginIndicator)

  public:
    enum ReturnCode
    {
      _ERROR,
      _OK,
      _CANCEL,
      _TIMEOUT
    };
    
    DialogUser * dialog (QWidget *, Entity *);
    int run (Entity *, QList<Curve *> &, QList<Marker *> &);
    Entity * settings ();
    int runExternal (QString com, QString file, QList<Curve *> &, QList<Marker *> &);
    void done (int rc, QString file);
    int verbNew (CommandParse &, QString &);
    int verbRun (CommandParse &, QList<Curve *> &, QList<Marker *> &, QString &);
    int verbGet (CommandParse &, QString &);
    int verbSet (CommandParse &, QString &);
    
  private:
    bool _killFlag;
    QHash<QString, Entity *> _settings;
};

#endif
