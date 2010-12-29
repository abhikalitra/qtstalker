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

#ifndef PLOT_PLUGIN_HPP
#define PLOT_PLUGIN_HPP

#include "ScriptPlugin.h"

class PLOT : public ScriptPlugin
{
  public:
    enum Method
    {
      _BACKGROUND_COLOR,
      _FONT,
      _GRID_COLOR,
      _GRID,
      _CROSSHAIRS_COLOR,
      _CROSSHAIRS,
      __NEW,
      _DELETE,
      _UPDATE
    };
    
    PLOT ();
    int command (Command *);
    int backgroundColor (Command *);
    int font (Command *);
    int gridColor (Command *);
    int grid (Command *);
    int crosshairsColor (Command *);
    int crosshairs (Command *);
    int newPlot (Command *);
    int deletePlot (Command *);
    int update (Command *);

  private:
    QStringList _method;
};

extern "C"
{
  ScriptPlugin * createScriptPlugin ();
}

#endif
