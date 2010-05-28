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

#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <QString>
#include <QList>
#include <QHash>

#include "PlotLine.h"
#include "Setting.h"
#include "COPlugin.h"

class Indicator
{
  public:
    Indicator ();
    void setName (QString &);
    QString & name ();
    void setEnable (int);
    int enable ();
    void setTabRow (int);
    int tabRow ();
    void setDate (int);
    int date ();
    void setLog (int);
    int log ();
    void setCUS (int);
    int CUS ();
    void setIndicator (QString &);
    QString & indicator ();
    void setLine (QString &, PlotLine *);
    PlotLine * line (QString &);
    void setSettings (Setting &);
    Setting & settings ();
    void clear ();
    void setChartObjects (QHash<QString, COPlugin *> &);
    void getChartObjects (QHash<QString, COPlugin *> &);
    void addChartObject (COPlugin *);
    void clearChartObjects ();
    void deleteChartObject (QString &);

    void addPlotOrder (QString &);
    QStringList & plotOrder ();
    void weedPlots ();
    void cleanClear ();
    void init ();

  protected:
    QHash<QString, PlotLine *> _lines;
    QHash<QString, COPlugin *> _chartObjects;
    Setting _settings;
    QString _name;
    QString _indicator;
    QStringList _plotOrder;
    int _enable;
    int _tabRow;
    int _date;
    int _log;
    int _cus;
};

#endif
