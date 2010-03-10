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

#ifndef COLOR_HPP
#define COLOR_HPP

#include "IndicatorPlugin.h"

class COLOR : public IndicatorPlugin
{
  public:
    COLOR ();
    int getCUS (QStringList &set, QHash<QString, PlotLine *> &tlines, BarData *data);
    int getCompare (QStringList &set, QHash<QString, PlotLine *> &tlines);
    int getAll (QStringList &set, QHash<QString, PlotLine *> &tlines);
    
  protected:
    QStringList methodList;
};

#endif
