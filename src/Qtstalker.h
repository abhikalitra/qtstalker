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
#include <QTabWidget>
#include <QTextEdit>
#include <QHash>
#include <QStatusBar>
#include <QList>
#include <QSpinBox>
#include <QTimer>
#include <QButtonGroup>

#include "Config.h"
#include "Plot.h"
#include "ChartPage.h"
#include "BarData.h"
#include "InfoPanel.h"
#include "ScriptPage.h"
#include "PlotSlider.h"
#include "COToolBar.h"
#include "ZoomButtons.h"
#include "BarLengthButtons.h"
#include "RecentCharts.h"
#include "GroupPage.h"
#include "PluginPage.h"
#include "GridAction.h"
#include "RefreshAction.h"
#include "DocsAction.h"
#include "TabWidget.h"
#include "Splitter.h"
#include "DateRangeControl.h"
#include "DateRangeButton.h"

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalPixelspace (int);
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalPlotFont (QFont);
    void signalIndex (int);
    void signalInterval(Bar::BarLength);
    void signalClearIndicator ();
    void signalGrid (bool);
    void signalNewIndicator ();
    void signalNewExternalChartObject (QString);
    void signalSetExternalChartObject ();
    void signalCursorChanged (int);
    void signalRefreshUpdated (int);
    
    void signalDeleteIndicatorTab (QString);

    void signalSaveSettings ();
    void signalLoadSettings ();

  public:
    QtstalkerApp (QString session, QString asset);
    void createGUI (Config &);
    void loadSettings (Config &);
    void createToolBars ();
    void initGroupNav ();
    void initChartNav ();
    void initIndicatorNav ();
    void initScriptNav ();
    void initPluginNav ();
    QString getWindowCaption ();
    void setSliderStart (int);

  public slots:
    void loadChart (BarData);
    void quit();
    void dataWindow ();
    void deleteIndicator (QString);
    void barLengthChanged (int);
    void chartUpdated ();
    void statusMessage (QString);
    void addIndicator (QString);
    void drawPlots ();
    void addIndicatorButton (QString);
    void wakeup ();
    void appFont (QFont);
    void psButtonClicked (int);
    void zoomChanged(int, int);
    void commandLineAsset ();
    void sliderChanged (int);
    void gridChanged (bool);
    void backgroundColorChanged (QColor);
    void borderColorChanged (QColor);
    void plotFontChanged (QFont);

  protected:
    Splitter *_split;
    Splitter *_navSplitter;
    Splitter *_dpSplitter;
    QTabWidget *_navTab;
    QWidget *_baseWidget;
    QWidget *_navBase;
    ChartPage *_chartNav;
    QHash<QString, Plot *> _plotList;
    InfoPanel *_infoPanel;
    QStatusBar *_statusBar;
    QList<TabWidget*> _tabList;
    RecentCharts *_recentCharts;
    BarLengthButtons *_barLengthButtons;
    BarData _currentChart;
    ScriptPage *_scriptPage;
    PlotSlider *_plotSlider;
    COToolBar *_toolBar2;
    ZoomButtons *_zoomButtons;
    QString _clAsset;
    GroupPage *_groupNav;
    PluginPage *_pluginNav;
    GridAction *_gridAction;
    RefreshAction *_refreshAction;
    DateRangeControl *_dateRange;
    DateRangeButton *_dateRangeButton;
};

#endif

