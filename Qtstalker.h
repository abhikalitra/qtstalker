/***************************************************************************
                          qtstalker.h  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001,2002 by Stefan Stratigakos
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

#include "Indicator.h"
#include "Plot.h"
#include "Config.h"

#include <qmainwindow.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qscrollview.h>
#include <qcombobox.h>
#include <qlist.h>
#include <qarray.h>
#include <qslider.h>
#include <qtabbar.h>
#include <qspinbox.h>

#define DEFAULT_INDICATOR_HEIGHT 125

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  public:

    enum chartStatus
    {
      None,
      Chart,
      Group
    };

    QtstalkerApp();
    ~QtstalkerApp();
    void initConfig ();
    void initActions();
    void initMenuBar();
    void initToolBar();
    QString getWindowCaption ();
    void setChartType (int);
    void loadChart (QString);
    void compressionChanged (QString);
    void addIndicatorButton (QString);
    void setPlotColors ();

  public slots:

    void slotAbout ();
    void slotQuit();
    void slotWorkwithChart ();
    void slotWorkwithGroup ();
    void slotWorkwithPortfolio ();
    void slotOpenChart (QString);
    void slotOpenGroup (QString);
    void slotQuotes ();
    void slotOptions ();
    void slotBack ();
    void slotNext ();
    void slotDataWindow ();
    void slotGroupComboChanged (int);
    void slotBarComboChanged (int);
    void slotNewIndicator ();
    void slotEditIndicator (int);
    void slotDeleteIndicator (int);
    void slotNewChartObject (int);
    void slotEditChartObject (int);
    void slotDeleteChartObject (int);
    void slotGrid (bool);
    void slotCompressionChanged (QAction *);
    void slotChartTypeChanged (QAction *);
    void slotSliderChanged (int);
    void plotPopupMenu (int);
    void mainPlotPopupMenu ();
    void indicatorPlotPopupMenu ();
    void slotPixelspaceChanged (int);
    void slotScaleToScreen (bool);
    void slotIndicatorSelected (int);
    void slotNewPlugin ();
    void slotChartUpdated ();
    void slotStatusMessage (QString);

  private:
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *viewMenu;
    QPopupMenu *pluginMenu;
    QPopupMenu *toolMenu;
    QPopupMenu *helpMenu;

    QPopupMenu *chartMenu;
    QPopupMenu *chartEditMenu;
    QPopupMenu *chartDeleteMenu;
    QPopupMenu *chartObjectDeleteMenu;
    QPopupMenu *chartObjectEditMenu;

    QAction *actionQuit;
    QAction *actionAbout;
    QAction *actionWorkwithChart;
    QAction *actionWorkwithGroup;
    QAction *actionOptions;
    QAction *actionDaily;
    QAction *actionWeekly;
    QAction *actionMonthly;
    QAction *actionGrid;
    QAction *actionDatawindow;
    QAction *actionBack;
    QAction *actionNext;
    QAction *actionNewIndicator;
    QAction *actionPortfolio;
    QAction *actionQuotes;
    QAction *actionScaleToScreen;
    QAction *actionNewPlugin;
    QAction *actionTester;
    QAction *actionBar;
    QAction *actionPaintBar;
    QAction *actionLine;
    QAction *actionCandle;
    QAction *actionPoint;

    QActionGroup *compressionGroupAction;
    QActionGroup *chartTypeGroupAction;

    QScrollView *view;
    QToolBar *toolbar;
    QToolBar *navToolbar;
    QMenuBar *menubar;
    Plot *mainPlot;
    Plot *indicatorPlot;
    QComboBox *barCombo;
    QComboBox *groupCombo;
    QSpinBox *pixelspace;
    Config *config;
    QSlider *slider;
    chartStatus status;
    QString chartPath;
    QString chartName;
    QString chartType;
    QString chartSymbol;
    QString indicatorGroupText;
    int chartObjectId;
    QTabBar *tabBar;
    QList<QTab> tabList;
};

#endif 

