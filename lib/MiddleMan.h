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

#ifndef MIDDLE_MAN_HPP
#define MIDDLE_MAN_HPP

#include <QObject>
#include <QStringList>
#include <QColor>
#include <QFont>

class MiddleMan: public QObject
{
  Q_OBJECT
    
  signals:
    void signalChartPanelRefresh ();
    void signalChartPanelSearch (QString, QString);
    void signalGroupPanelRefresh ();
    void signalPlotBackgroundColor (QColor);
    void signalGridColor (QColor);
    void signalGrid (bool);
    void signalCrosshairsColor (QColor);
    void signalCrosshairs (bool);
    void signalIndicatorNew (QString);
    void signalIndicatorDelete (QStringList);
    void signalScriptRun (QString);
    void signalPlotTabPosition (int);
    void signalPlotUpdate (QString);
    void signalChartObjectDelete (QStringList);
    void signalChartObjectUpdate (QString);
    void signalStatusMessage (QString);
    void signalChartObjectNew (QString, QString, QString);

  public:
    MiddleMan (QObject *);
    void chartPanelRefresh ();
    void chartPanelSearch (QString, QString);
    void groupPanelRefresh ();
    void plotBackgroundColor (QColor);
    void gridColor (QColor);
    void grid (int);
    void crosshairsColor (QColor);
    void crosshairs (int);
    void indicatorNew (QString);
    void indicatorDelete (QStringList);
    void scriptRun (QString);
    void plotTabPosition (int);
    void plotUpdate (QString);
    void chartObjectDelete (QStringList);
    void chartObjectUpdate (QString);
    void statusMessage (QString);
    void chartObjectNew (QString indicator, QString name, QString type);
};

#endif
