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

#ifndef FIBOLINE_HPP
#define FIBOLINE_HPP

#include "ChartObject.h"

class FiboLine : public ChartObject
{
  Q_OBJECT

  public:
    FiboLine (Scaler *, QPixmap *, QString, QString, QString, QString, QString, QString);
    ~FiboLine ();
    void draw (int, int);
    double getY (QString, double, double, double);
    void move (QString, QString);
    QString getDate ();
    QString getDate2 ();
    bool isClicked (int, int);
    
  public slots:
    void prefDialog ();
    
  protected:
    QRegion area2;
    bool move2Flag;
};

#endif
