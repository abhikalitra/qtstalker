/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef SELLARROWOBJECT_HPP
#define SELLARROWOBJECT_HPP

#include <qregion.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qpoint.h>
#include "BarDate.h"
#include "Setting.h"

class SellArrowObject
{
  public:
    enum Status
    {
      Plot,
      Selected,
      Delete
    };
  
    SellArrowObject (QString, QString, BarDate, double);
    SellArrowObject ();
    ~SellArrowObject ();
    void init ();
    void setSaveFlag (bool);
    bool getSaveFlag ();
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    QString getPlot ();
    QString getName ();
    void setDate (BarDate);
    BarDate getDate ();
    void setColor (QColor);
    QColor getColor ();
    void setValue (double);
    double getValue ();
    bool isSelected (QPoint point);
    bool isGrabSelected (QPoint point);
    void setStatus (Status);
    SellArrowObject::Status getStatus ();
    Setting * getSettings ();
    void setSettings (Setting *);
    
  protected:
    bool saveFlag;
    QPtrList<QRegion> grabHandles;
    QPtrList<QRegion> selectionArea;
    QString plot;
    QString name;
    QColor color;
    BarDate date;
    double value;
    Status status;
};

#endif