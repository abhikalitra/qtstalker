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
    void signalPlotFont (QFont);
    void signalGridColor (QColor);
    void signalGrid (bool);
    void signalCrosshairsColor (QColor);
    void signalCrosshairs (bool);
    void signalIndicatorNew (QString);
    void signalIndicatorDelete (QStringList);
    void signalScriptRun (QString);

  public:
    enum Type
    {
      _CHART_PANEL_REFRESH,
      _CHART_PANEL_SEARCH,
      _GROUP_PANEL_REFRESH,
      _PLOT_BACKGROUND_COLOR,
      _PLOT_FONT,
      _GRID_COLOR,
      _GRID,
      _CROSSHAIRS_COLOR,
      _CROSSHAIRS,
      _INDICATOR_NEW,
      _INDICATOR_DELETE,
      _SCRIPT_RUN
    };
    
    MiddleMan (QObject *);
    void chartPanelRefresh ();
    void chartPanelSearch (QString, QString);
    void groupPanelRefresh ();
    void plotBackgroundColor (QColor);
    void plotFont (QFont);
    void gridColor (QColor);
    void grid (int);
    void crosshairsColor (QColor);
    void crosshairs (int);
    void indicatorNew (QString);
    void indicatorDelete (QStringList);
    void scriptRun (QString);
};

#endif
