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
#include <qaction.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qptrlist.h>
#include <qmultilineedit.h>

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

#define DEFAULT_INDICATOR_HEIGHT 125

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT
  
  signals:
    void signalGrid (bool);
    void signalScaleToScreen (bool);
    void signalPixelspace (int);
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalPlotFont (QFont);
    void signalIndex (int);
    void signalInterval(BarData::BarCompression);
    void signalIndicatorPageRefresh ();
    void signalPlotDate (bool);
    void signalChartPath (QString);
    void signalDrawMode (bool);
    void signalCrosshairsStatus(bool);

  public:

    enum chartStatus
    {
      None,
      Chart
    };

    QtstalkerApp ();
    ~QtstalkerApp ();
    void initConfig ();
    void initActions ();
    void initMenuBar ();
    void initToolBar ();
    void initGroupNav ();
    void initChartNav ();
    void initPortfolioNav();
    void initTestNav();
    void initIndicatorNav ();
    void initScannerNav ();
    QString getWindowCaption ();
    void setChartType (int);
    void loadChart (QString);
    void compressionChanged ();
    void addIndicatorButton (QString, Indicator::PlotType);
    void exportChart (QString);
    void traverse(QString);
    void loadIndicator (Indicator *);
    void setSliderStart (int, bool);

  public slots:

    void slotAbout ();
    void slotQuit();
    void slotOpenChart (QString);
    void slotQuotes ();
    void slotOptions ();
    void slotDataWindow ();
    void slotNewIndicator ();
    void slotEditIndicator (QString, Plot *);
    void slotDeleteIndicator (QString, Plot *);
    void slotGrid (bool);
    void slotCompressionChanged (int);
    void slotChartTypeChanged (int);
    void slotSliderChanged (int);
    void slotPixelspaceChanged (int);
    void slotScaleToScreen (bool);
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void slotTabChanged (QWidget *);
    void slotHideNav (bool);
    void slotUpdateInfo (Setting *);
    void slotLogScale (bool);
    void slotHideMainPlot (bool);
    void slotPlotDate (bool);
    void slotPlotLeftMouseButton (int, int, bool);
    void slotPlotKeyPressed (QKeyEvent *);
    void slotDrawMode (bool);
    void slotMinPixelspaceChanged (int);
    void slotCrosshairsStatus (bool);
    void slotNavigatorPosition (int);
    void slotMainPlotFocus ();
    void slotTabIndicatorFocus ();
    void slotNavigatorButtonPressed (int);
    void slotHelp ();
    void slotExitQuoteDialog ();

  private:
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *viewMenu;
    QPopupMenu *toolMenu;
    QPopupMenu *helpMenu;

    QAction *actionQuit;
    QAction *actionAbout;
    QAction *actionOptions;
    QAction *actionGrid;
    QAction *actionDatawindow;
    QAction *actionNewIndicator;
    QAction *actionQuotes;
    QAction *actionScaleToScreen;
    QAction *actionNav;
    QAction *actionLogScale;
    QAction *actionHideMainPlot;
    QAction *actionPlotDate;
    QAction *actionDrawMode;
    QAction *actionPlotFocus;
    QAction *actionTabIndicatorFocus;
    QAction *actionHelp;
    
    QToolBar *toolbar;
    QToolBar *toolbar2;
    QMenuBar *menubar;
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
    QSpinBox *barCount;
    QComboBox *compressionCombo;
    QComboBox *chartTypeCombo;
    QSpinBox *pixelspace;
    Config config;
    QSlider *slider;
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
};

#endif

