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
#include <qdict.h>
#include <qlist.h>
#include "Setting.h"
#include "PlotLine.h"

class Indicator : public Setting
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
    QStringList getChartObjects ();
    Setting * getChartObject (QString);
    void addChartObject (Setting *);
    void deleteChartObject (QString);
    bool getMainPlot ();
    void clearLines ();
    void setEnable (bool);
    bool getEnable ();

  private:
    QList<PlotLine> lines;
    QMemArray<int> alerts;
    QDict<Setting> chartObjects;
    bool enable;
};

#endif

