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

// *************************************************************************************************
// Base class for chart objects. Buy,Sell etc inherit this.
// *************************************************************************************************

#ifndef CHART_OBJECT_HPP
#define CHART_OBJECT_HPP

#include <QStringList>
#include <QDateTime>
#include <QPoint>
#include <QPainter>

#include "Setting.h"
#include "ChartObjectDialog.h"
#include "ChartObjectSettings.h"
#include <qwt_plot_item.h>
#include <qwt_scale_map.h>

class ChartObject : public QwtPlotItem
{
  public:
    enum Type
    {
      _Buy,
      _HLine,
      _Retracement,
      _Sell,
      _Text,
      _TLine,
      _VLine
    };

    ChartObject ();
    virtual ~ChartObject ();
    virtual void info (Setting &);
    virtual ChartObjectDialog * dialog ();
    virtual void load ();
    virtual int CUS (QStringList &);
    virtual int isSelected (QPoint);
    virtual int highLow (int start, int end, double &high, double &low);
    void setSettings (ChartObjectSettings &);
    void settings (ChartObjectSettings &);

  protected:
    ChartObjectSettings _settings;
    mutable QList<QRegion> _selectionArea;
//    mutable QList<QRegion> _grabHandles;
};

#endif
