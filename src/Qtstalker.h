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

#include <qmainwindow.h>
#include <qaction.h>
#include <qmenubar.h>
#include <qpopupmenu.h>
#include <qtoolbar.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qsplitter.h>
#include <qtoolbutton.h>
#include <qlist.h>
#include <qmultilineedit.h>

#include "Indicator.h"
#include "Plot.h"
#include "Config.h"
#include "Navigator.h"
#include "Setting.h"
#include "ChartPage.h"

#define DEFAULT_INDICATOR_HEIGHT 125

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

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
    QString getWindowCaption ();
    void setChartType (int);
    void loadChart (QString);
    void compressionChanged (QString);
    void addIndicatorButton (QString);
    void setPlotFont (Plot *);
    void setPlotColor (Plot *, Config::Parm);
    void exportChart (QString);
    void traverse(QString);

  public slots:

    void slotAbout ();
    void slotQuit();
    void slotOpenChart (QString);
    void slotQuotes ();
    void slotOptions ();
    void slotDataWindow ();
    void slotBarComboChanged (int);
    void slotNewIndicator ();
    void slotEditIndicator (int);
    void slotDeleteIndicator (int);
    void slotNewChartObject (int);
    void slotEditChartObject (int);
    void slotDeleteChartObject (int);
    void slotGrid (bool);
    void slotCompressionChanged (int);
    void slotChartTypeChanged (int);
    void slotSliderChanged (int);
    void plotPopupMenu (int);
    void mainPlotPopupMenu ();
    void indicatorPlotPopupMenu ();
    void slotPixelspaceChanged (int);
    void slotScaleToScreen (bool);
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void slotTabChanged (QWidget *);
    void slotChartObjectCreated (Setting *);
    void slotHideNav (bool);
    void slotUpdateInfo (Setting *);
    void slotLogScale (bool);

  private:
    QPopupMenu *fileMenu;
    QPopupMenu *editMenu;
    QPopupMenu *viewMenu;
    QPopupMenu *toolMenu;
    QPopupMenu *helpMenu;

    QPopupMenu *chartMenu;
    QPopupMenu *chartEditMenu;
    QPopupMenu *chartDeleteMenu;
    QPopupMenu *chartObjectDeleteMenu;
    QPopupMenu *chartObjectEditMenu;
    QPopupMenu *chartObjectMenu;

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

    QToolBar *toolbar;
    QMenuBar *menubar;
    QSplitter *split;
    QSplitter *navSplitter;
    QTabWidget *tabs;
    QTabWidget *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    Plot *mainPlot;
    QDict<Plot> plotList;
    QComboBox *barCombo;
    QComboBox *compressionCombo;
    QComboBox *chartTypeCombo;
    QSpinBox *pixelspace;
    Config *config;
    QSlider *slider;
    chartStatus status;
    QString chartPath;
    QString chartName;
    QString chartType;
    QString chartSymbol;
    int chartObjectId;
    QList<Setting> *recordList;
    QMultiLineEdit *infoLabel;
};

#endif 
