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

#ifndef PLOT_CURSOR_HPP
#define PLOT_CURSOR_HPP

#include <QObject>
#include <QString>
#include <QMouseEvent>
#include <QPainter>

#include "PlotData.h"
#include "Setting.h"
#include "DateBar.h"
#include "Indicator.h"

class PlotCursor : public QObject
{
  Q_OBJECT

  signals:
//    void signalMessage (QString);
    void signalInfoMessage (Setting *);
//    void signalUpdate ();
    void signalPixelSpaceChanged (int, int);
    void signalCOSelected (int);
    void signalIndexChanged (int);
    
  public:
    PlotCursor ();
    ~PlotCursor ();
    virtual int getCursor ();
    virtual void draw (QPainter &, PlotData &, DateBar &, Scaler &);
    virtual void mousePress (PlotData &, DateBar &, Scaler &, Indicator &);
    virtual void mouseDoubleClick (PlotData &, DateBar &, Scaler &);
    virtual void mouseMove (PlotData &, DateBar &, Scaler &, Indicator &);
    
    QString & type ();
    int convertXToDataIndex (int x, PlotData &, DateBar &);

  protected:
    QString _type;
};

#endif

