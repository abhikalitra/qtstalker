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
#include <QSplitter>
#include <QAction>

#include "Config.h"
#include "Plot.h"
#include "BarData.h"
#include "InfoPanel.h"
#include "PlotSlider.h"
#include "COToolBar.h"
#include "ZoomButtons.h"
#include "BarLengthButtons.h"
#include "RecentCharts.h"
#include "GridAction.h"
#include "RefreshAction.h"
#include "DocsAction.h"
#include "Splitter.h"
#include "DateRangeControl.h"
#include "DateRangeButton.h"
#include "SidePanel.h"
#include "ChartLayout.h"
#include "SidePanelButton.h"

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalSaveSettings ();
    void signalLoadSettings ();

  public:
    QtstalkerApp (QString session, QString asset);
    void createGUI (Config &);
    void loadSettings (Config &);
    void createToolBars ();
    QString getWindowCaption ();
    void setSliderStart (int);

  public slots:
    void loadChart (BarData);
    void loadChart2 (QString);
    void quit();
    void dataWindow ();
    void chartUpdated ();
    void statusMessage (QString);
    void wakeup ();
    void appFont (QFont);
    void commandLineAsset ();

  protected:
    QSplitter *_split;
    Splitter *_navSplitter;
    QWidget *_baseWidget;
    InfoPanel *_infoPanel;
    QStatusBar *_statusBar;
    RecentCharts *_recentCharts;
    BarLengthButtons *_barLengthButtons;
    BarData _currentChart;
    PlotSlider *_plotSlider;
    COToolBar *_toolBar2;
    ZoomButtons *_zoomButtons;
    QString _clAsset;
    GridAction *_gridAction;
    RefreshAction *_refreshAction;
    DateRangeControl *_dateRange;
    DateRangeButton *_dateRangeButton;
    SidePanel *_sidePanel;
    ChartLayout *_chartLayout;
    QAction *_newIndicatorAction;
    SidePanelButton *_sidePanelButton;
};

#endif

