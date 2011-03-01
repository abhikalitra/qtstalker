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
#include <QObject>

#include "Curve.h"
#include "Setting.h"

class Indicator : public QObject
{
  Q_OBJECT

  signals:
    void signalPlot ();
  
  public:
    Indicator (QObject *);
    Indicator ();
    QString & name ();
    bool lock ();
    bool date ();
    bool log ();
    QString & script ();
    QString & command ();
    void setLine (QString, Curve *);
    void setLine (int, Curve *);
    Curve * line (QString);
    QHash<QString, Curve *> & curves ();
    void clearLines ();
    int deleteLine (QString);
    void clear ();
    Setting chartObject (QString);
    void addChartObject (Setting);
    void clearChartObjects ();
    void weedPlots ();
    void init ();
    void loadChartObjects ();
    QHash<QString, Setting> & chartObjects ();
    int chartObjectCount ();
    int lineCount ();

  public slots:
    int save ();
    int load ();
    void setName (QString);
    void setLock (bool);
    void setDate (bool);
    void setLog (bool);
    void setScript (QString);
    void setCommand (QString);
    void calculate ();
    void scriptFinished ();

  protected:
    QHash<QString, Curve *> _lines;
    QHash<QString, Setting> _chartObjects;
    QString _name;
    QString _script;
    QString _command;
    bool _lock;
    bool _date;
    bool _log;
};

// this is for passing Indicator data between threads
//Q_DECLARE_METATYPE(Indicator)

#endif
