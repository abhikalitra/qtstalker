/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#ifndef FIBOLINEOBJECT_HPP
#define FIBOLINEOBJECT_HPP

#include <qregion.h>
#include <qcolor.h>
#include <qptrlist.h>
#include <qpoint.h>
#include "BarDate.h"
#include "Setting.h"

class FiboLineObject
{
  public:
    enum Status
    {
      Plot,
      Selected,
      Delete
    };
  
    FiboLineObject (QString, QString, BarDate, double, BarDate, double);
    FiboLineObject ();
    ~FiboLineObject ();
    void init ();
    void setSaveFlag (bool);
    bool getSaveFlag ();
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    QString getPlot ();
    QString getName ();
    void setStartDate (BarDate);
    BarDate getStartDate ();
    void setEndDate (BarDate);
    BarDate getEndDate ();
    void setColor (QColor);
    QColor getColor ();
    void setHigh (double);
    double getHigh ();
    void setLow (double);
    double getLow ();
    bool isSelected (QPoint point);
    int isGrabSelected (QPoint point);
    void setStatus (Status);
    FiboLineObject::Status getStatus ();
    void getSettings (Setting &);
    void setSettings (Setting &);
    void setLine (int, double);
    double getLine (int);
    bool getExtend ();
    void setExtend (bool);
    
  protected:
    bool saveFlag;
    QPtrList<QRegion> grabHandles;
    QPtrList<QRegion> selectionArea;
    QString plot;
    QString name;
    QColor color;
    BarDate startDate;
    BarDate endDate;
    double high;
    double low;
    Status status;
    double line1;
    double line2;
    double line3;
    double line4;
    double line5;
    double line6;
    bool extend;
};

#endif
