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

#ifndef QTSTALKER_H
#define QTSTALKER_H

#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QString>
#include <QStatusBar>
#include <QHash>

#include "InfoPanel.h"
#include "RecentCharts.h"
#include "BarLengthButtons.h"
#include "PlotSlider.h"
#include "ZoomButtons.h"
#include "DateRangeControl.h"
#include "DateRangeButton.h"
#include "SidePanel.h"
#include "Command.h"
#include "QuitButton.h"
#include "Plot.h"

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalLoadSettings ();
    void signalClearPlot ();
    void signalPlot ();

  public:
    QtstalkerApp (QString session, QString asset);
    void createGUI ();
    void loadSettings ();
    void createToolBar ();
    void createStatusToolBar ();
    QString getWindowCaption ();
    void setSliderStart (int);
    void addPlot (QString);

  public slots:
    void loadChart (BarData);
    void save ();
    void chartUpdated ();
    void statusMessage (QString);
    void wakeup ();
    void commandLineAsset ();
    void addNewPlot (QString);
    void deletePlot (QStringList);
    void setPlotTabPosition (int);
    void fixDockTabs ();
    void updatePlot (QString);

  protected:
    QHash<QString, Plot *> _plots;
    InfoPanel *_infoPanel;
    QStatusBar *_statusBar;
    RecentCharts *_recentCharts;
    BarLengthButtons *_barLengthButtons;
    PlotSlider *_plotSlider;
    ZoomButtons *_zoomButtons;
    QString _clAsset;
    DateRangeControl *_dateRange;
    DateRangeButton *_dateRangeButton;
    SidePanel *_sidePanel;
    QuitButton *_quitButton;
    QToolBar *_statusToolBar;
};

#endif
