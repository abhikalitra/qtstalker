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

#ifndef PLUGIN_PO_HPP
#define PLUGIN_PO_HPP

#include "Plugin.h"

class PO : public Plugin
{
  Q_OBJECT

  public:
    enum Method
    {
      _APO,
      _PPO
    };

    enum Parm
    {
      _COLOR = 10,
      _STYLE = 20,
      _LABEL = 30,
      _PERIOD_FAST = 40,
      _PERIOD_SLOW = 50,
      _MA_TYPE = 60,
      _INPUT = 70,
      _METHOD = 80
    };

    PO ();
    int command (Command *);
    int calculate (BarData *, Indicator *);
    void defaults (Setting *);
    void dialog (QWidget *, Indicator *);
    QStringList method ();

  private:
    QStringList _method;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
