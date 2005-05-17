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

#ifndef SELLARROW_HPP
#define SELLARROW_HPP

#include "COPlugin.h"
#include "SellArrowObject.h"
#include "BarDate.h"
#include "Setting.h"
#include <qpointarray.h>
#include <qdict.h>
#include <qpoint.h>

class SellArrow : public COPlugin
{
  Q_OBJECT

  public:
    SellArrow ();
    ~SellArrow ();
    void draw (QPixmap &, Scaler &, int, int, int);
    void newObject (QString &, QString &);
    void addObject (Setting &);
    void saveObjects (QString &);
    void clear ();
    void keyEvent (QKeyEvent *);
    double getHigh ();
    double getLow ();
    void showMenu ();
    void getNameList (QStringList &);
    
    void loadDefaults ();
    void saveDefaults ();
    
  public slots:
    void prefDialog ();
    void moveObject ();
    void removeObject ();
    COPlugin::Status pointerClick (QPoint &, BarDate &, double);
    void pointerMoving (QPixmap &, QPoint &, BarDate &, double y);
    
  protected:
    QPointArray arrow;
    QDict<SellArrowObject> objects;
    Status status;
    SellArrowObject *selected;
    QColor defaultColor;
};

extern "C"
{
  COPlugin * createCOPlugin ();
}


#endif
