/***************************************************************************
                          qtstalker.h  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2004 by Stefan Stratigakos
    email                : 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef QTSTALKER_H
#define QTSTALKER_H

#include <qmainwindow.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qtabwidget.h>
#include <qsplitter.h>
#include <qmultilineedit.h>
#include <qdict.h>
#include <qprogressbar.h>
#include <qstatusbar.h>

#include "Indicator.h"
#include "Plot.h"
#include "Config.h"
#include "Navigator.h"
#include "Setting.h"
#include "ChartPage.h"
#include "IndicatorTab.h"
#include "NavigatorTab.h"
#include "Quote.h"
#include "IndicatorPage.h"
#include "ScannerPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "GroupPage.h"
#include "MacroPage.h"
#include "ChartToolbar.h"
#include "MainMenubar.h"
#include "Macro.h"

#define DEFAULT_INDICATOR_HEIGHT 125

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
    void signalInterval(BarData::BarCompression);
    void signalChartPath (QString);
    void signalCrosshairsStatus(bool);
    void signalSetKeyFlag (bool);

  public:

    enum chartStatus
    {
      None,
      Chart
    };

    QtstalkerApp ();
    ~QtstalkerApp ();
    void initConfig ();
    void initMenuBar ();
    void initToolBar ();
    void initGroupNav ();
    void initChartNav ();
    void initPortfolioNav();
    void initTestNav();
    void initIndicatorNav ();
    void initScannerNav ();
    void initMacroNav ();
    QString getWindowCaption ();
    void setChartType (int);
    void loadChart (QString &);
    void compressionChanged ();
    void addIndicatorButton (QString &, Indicator::PlotType);
    void exportChart (QString &);
    void traverse(QString &);
    void loadIndicator (Indicator *);
    void setSliderStart (int, bool);
    void initPlot (Plot *);

  public slots:
    void slotAbout ();
    void slotQuit();
    void slotOpenChart (QString);
    void slotQuotes ();
    void slotOptions ();
    void slotDataWindow ();
    void slotNewIndicator (Indicator *);
    void slotEditIndicator (Indicator *);
    void slotDeleteIndicator (QString);
    void slotCompressionChanged (int);
    void slotChartTypeChanged (int);
    void slotPixelspaceChanged (int);
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void slotHideNav (bool);
    void slotUpdateInfo (Setting *);
    void slotPlotLeftMouseButton (int, int, bool);
    void slotMinPixelspaceChanged (int);
    void slotCrosshairsStatus (bool);
    void slotNavigatorPosition (int);
    void slotHelp ();
    void slotExitQuoteDialog ();
    void slotDisableIndicator (QString);
    void slotEnableIndicator (QString);
    void slotRunMacro (QString);
    void slotRecordMacro (QString);
    void slotStopMacro ();
    void slotProgMessage (int, int);
    
  private:
    QToolBar *toolbar;
    ChartToolbar *toolbar2;
    MainMenubar *menubar;
    QSplitter *split;
    QSplitter *navSplitter;
    QSplitter *dpSplitter;
    IndicatorTab *tabs;
    NavigatorTab *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    Plot *mainPlot;
    QDict<Plot> plotList;
    Config config;
    chartStatus status;
    QString chartPath;
    QString chartName;
    QString chartType;
    QString chartSymbol;
    QString dbPlugin;
    BarData *recordList;
    QMultiLineEdit *infoLabel;
    QuoteDialog *quoteDialog;
    IndicatorPage *ip;
    PortfolioPage *pp;
    ScannerPage *sp;
    TestPage *tp;
    GroupPage *gp;
    MacroPage *mp;
    Macro *currentMacro;
    QProgressBar *progBar;
    QStatusBar *statusbar;
};

#endif

