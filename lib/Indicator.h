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
#include <QMap>
#include <QHash>
#include <QMetaType>

#include "Curve.h"
#include "Setting.h"

class Indicator
{
  public:
    Indicator ();
    void setName (QString);
    QString & name ();
    void setTabRow (int);
    int tabRow ();
    void setDate (int);
    int date ();
    void setLog (int);
    int getLog ();
    void setScript (QString);
    QString & script ();
    void setCommand (QString);
    QString & command ();
    void setLine (QString, Curve *);
    void setLine (int, Curve *);
    Curve * line (QString);
    QHash<QString, Curve *> & curves ();
    void clearLines ();
    void deleteLines ();
    int deleteLine (QString);
    void clear ();
    Setting chartObject (int);
    void addChartObject (Setting &);
    void clearChartObjects ();
    void deleteChartObject (int);
    void weedPlots ();
    void clean ();
    void init ();
    void coKeys (QList<int> &);
    int coCount ();

  protected:
    QHash<QString, Curve *> _lines;
    QMap<int, Setting> _chartObjects;
    QString _name;
    QString _script;
    QString _command;
    int _tabRow;
    int _date;
    int _log;
};

// this is for passing Indicator data between threads
Q_DECLARE_METATYPE(Indicator)

#endif
