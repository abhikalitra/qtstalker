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

#ifndef PLUGIN_HT_SINE_HPP
#define PLUGIN_HT_SINE_HPP

#include "Plugin.h"

class HT_SINE : public Plugin
{
  Q_OBJECT

  public:
    HT_SINE ();
    int command (Command *);
    void defaults (QString &);
    PluginWidget * dialog (QWidget *);
    QList<Curve *> getSINE (QList<Curve *> &);
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
