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

#ifndef CYCLE_HPP
#define CYCLE_HPP

#include "COBase.h"
#include "Setting.h"
#include <qpoint.h>
#include <qdatetime.h>
//Added by qt3to4:
#include <QPixmap>

class Cycle : public COBase
{
  Q_OBJECT

  public:
    Cycle ();
    ~Cycle ();
    void draw (QPixmap &, Scaler &, int, int, int);
    void newObject (QString &, QString &);
    double getHigh ();
    double getLow ();
    void loadDefaults ();
    void saveDefaults ();
    void getSettings (Setting &);
    void setSettings (Setting &);
    bool isGrabSelected (QPoint point);
    
  public slots:
    void prefDialog ();
    COBase::Status pointerClick (QPoint &, QDateTime &, double);
    void pointerMoving (QPixmap &, QPoint &, QDateTime &, double y);
    
  protected:
    int defaultInterval;
    QPoint tpoint;
    int tpixelspace;
    int interval;
    int grabPosition;
    QString intervalLabel;
};

#endif
