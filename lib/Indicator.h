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

#include "Scaler.h"
#include "PlotData.h"
#include "DateBar.h"

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
    void setColumn (int);
    int column ();
    void setDate (int);
    int date ();
    void setLog (int);
    int getLog ();
    void setCUS (int);
    int CUS ();
    void setIndicator (QString &);
    QString & indicator ();
    void setLine (QString &, PlotLine *);
    PlotLine * line (QString &);
    int deleteLine (QString &);
    void setSettings (Setting &);
    Setting & settings ();
    void clear ();
    COPlugin * chartObject (int);
    void addChartObject (COPlugin *);
    void clearChartObjects ();
    void deleteChartObject (int);

    void addPlotOrder (QString &);
    QStringList & plotOrder ();
    void weedPlots ();
    void cleanClear ();
    void init ();
    void coKeys (QList<int> &);
    Scaler & scaler ();
    void setScale (PlotData &, DateBar &);

  protected:
    QHash<QString, PlotLine *> _lines;
    QHash<int, COPlugin *> _chartObjects;
    Setting _settings;
    QString _name;
    QString _indicator;
    QStringList _plotOrder;
    int _enable;
    int _tabRow;
    int _date;
    int _log;
    int _cus;
    int _column;
    Scaler _scaler;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(Indicator)

#endif
