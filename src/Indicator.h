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

#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <qstring.h>
#include <qstringlist.h>
#include <qmemarray.h>
#include <qptrlist.h>
#include "PlotLine.h"

class Indicator
{
  public:
    Indicator ();
    ~Indicator ();
    void clear ();
    QMemArray<int> getAlerts ();
    void setAlerts (QMemArray<int>);
    int getAlert (int);
    int getLines ();
    void addLine (PlotLine *);
    PlotLine * getLine (int);
    bool getMainPlot ();
    void setMainPlot (bool);
    void clearLines ();
    void setEnable (bool);
    bool getEnable ();
    void setName (QString);
    QString getName ();
    void setType (QString);
    QString getType ();
    void setFile (QString);
    QString getFile ();

  private:
    QPtrList<PlotLine> lines;
    QMemArray<int> alerts;
    bool enable;
    QString name;
    QString type;
    QString file;
    bool plot;
};

#endif

