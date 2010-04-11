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
#include <QSplitter>
#include <QTextEdit>
#include <QHash>
#include <QStatusBar>
#include <QList>
#include <QSpinBox>
#include <QTimer>
#include <QButtonGroup>

#include "Plot.h"
#include "ChartPage.h"
#include "assistant.h"
#include "BarData.h"
#include "InfoPanel.h"
#include "ScriptPage.h"
#include "PlotSlider.h"
#include "COToolBar.h"
#include "ZoomButtons.h"
#include "BarLengthButtons.h"
#include "RecentCharts.h"

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
    void signalScale (bool);
    void signalNewIndicator ();
    void signalNewExternalChartObject (QString);
    void signalSetExternalChartObject ();
    void signalCursorChanged (int);
    void signalRefreshUpdated (int);

  public:
    enum MenuAction
    {
      Exit,
      NewIndicator,
      Options,
      Grid,
      DataWindow1,
      About,
      ScaleToScreen,
      SidePanel,
      Help
    };

    QtstalkerApp (QString session, QString asset);
    void createActions ();
    void createMenuBar ();
    void createToolBars ();
    void initGroupNav ();
    void initChartNav ();
    void initIndicatorNav ();
    void initScriptNav ();
    QString getWindowCaption ();
    void setSliderStart (int);

  public slots:
    void loadChart (BarData *);
    void slotAbout ();
    void slotStartDocumentation ();
    void slotShowDocumentation (QString);
    void slotQuit();
    void slotOptions ();
    void slotDataWindow ();
    void slotDeleteIndicator (QString);
    void barLengthChanged (int);
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void addIndicator (QString);
    void slotDrawPlots ();
    void addIndicatorButton (QString);
    void wakeup ();
    void slotAppFont (QFont);
    void slotHideNav (bool d);
    void loadIndicator (BarData *, QString &);
    void psButtonClicked (int);
    void zoomChanged(int, int);
    void refreshChart ();
    void commandLineAsset ();

  protected:
    QSplitter *split;
    QSplitter *navSplitter;
    QSplitter *dpSplitter;
    QTabWidget *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    QHash<QString, Plot *> plotList;
    InfoPanel *infoPanel;
    QStatusBar *statusbar;
    QList<QTabWidget*> tabList;
    QHash<MenuAction, QAction*> actionList;
    QSpinBox *barCount;
    RecentCharts *recentCharts;
    Assistant *assistant;
    BarLengthButtons *barLengthButtons;
    BarData currentChart;
    ScriptPage *scriptPage;
    PlotSlider *plotSlider;
    COToolBar *toolBar2;
    ZoomButtons *zoomButtons;
    QString clAsset;
};

#endif

