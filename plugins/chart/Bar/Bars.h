/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2003 Stefan S. Stratigakos
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

#ifndef BARS_HPP
#define BARS_HPP

#include "ChartPlugin.h"
#include <qcolor.h>

class Bars : public ChartPlugin
{
  Q_OBJECT

  public:
    Bars ();
    virtual ~Bars ();
    void drawChart (int startX, int startIndex, int pixelspace);
    void drawBars (int startX, int startIndex, int pixelspace);
//    void drawPaintBars (int startX, int startIndex, int pixelspace);
    void prefDialog ();
    void loadSettings ();
    void saveSettings ();

  protected:
    QString style;
    QColor barNeutralColor;
    QColor barUpColor;
    QColor barDownColor;
    QColor paintUpColor;
    QColor paintDownColor;
};

extern "C"
{
  Plugin * create ();
};

#endif
