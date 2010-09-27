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

#ifndef CHART_OBJECT_DRAW_HPP
#define CHART_OBJECT_DRAW_HPP

#include <QList>
#include <QRegion>
#include <qwt_plot_item.h>

#include "ChartObjectSettings.h"

class ChartObjectDraw : public QwtPlotItem
{
  public:
    ChartObjectDraw ();
    ~ChartObjectDraw ();
    int rtti () const;
    void setSettings (ChartObjectSettings &);
    int isSelected (QPoint);
    int isGrabSelected (QPoint);
    void setSelected (int);

  protected:
    ChartObjectSettings _settings;
    int _selected;
    int _handleWidth;
    mutable QList<QRegion> _selectionArea;
    mutable QList<QRegion> _grabHandles;
};

#endif
