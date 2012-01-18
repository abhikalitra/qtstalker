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

#ifndef CHART_OBJECT_HPP
#define CHART_OBJECT_HPP

#include <QList>
#include <QRegion>
#include <QPainter>
#include <qwt_plot_item.h>
#include <qwt_scale_map.h>

#include "Entity.h"

class ChartObject : public QwtPlotItem
{
  public:
    enum Status
    {
      _NONE,
      _SELECTED,
      _MOVE,
      _MOVE2
    };

    ChartObject ();
    ~ChartObject ();
    virtual int info (Entity &);
    virtual int highLow (int start, int end, double &high, double &low);
    virtual void move (QPoint);
    virtual void click (int, QPoint);
    virtual int create ();

    int rtti () const;
    int isSelected (QPoint);
    int isGrabSelected (QPoint);
    void setSelected (int);
    void setSettings (Entity);
    Entity settings ();
    void setScript (QString);
    QString script ();
    int save ();
    void setModified (int);
    int modified ();
    void newID (QString &);

  protected:
    mutable Entity _settings;
    mutable QList<QRegion> _selectionArea;
    mutable QList<QRegion> _grabHandles;
    int _selected;
    int _handleWidth;
    Status _status;
    QString _script;
    int _modified;
};

#endif
