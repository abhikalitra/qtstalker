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
#include <qlabel.h>

#include "Indicator.h"
#include "Plot.h"
#include "Config.h"
#include "Navigator.h"

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
    QString getWindowCaption ();
    void setChartType (int);
    void loadChart (QString);
    void compressionChanged (QString);
    void addIndicatorButton (QString);
    void setPlotFont (Plot *);
    void setPlotColor (Plot *, Config::Parm);

  public slots:

    void slotAbout ();
    void slotQuit();
    void slotWorkwithChart ();
    void slotWorkwithGroup ();
    void slotWorkwithPortfolio ();
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
    void slotNewPlugin ();
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void slotWorkwithTest ();
    void slotTabChanged (QWidget *);
    void slotChartObjectCreated (Setting *);
    void slotChartSelected (QString);
    void slotGroupChanged ();

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
    QPopupMenu *chartObjectMenu;

    QAction *actionQuit;
    QAction *actionAbout;
    QAction *actionWorkwithChart;
    QAction *actionWorkwithGroup;
    QAction *actionOptions;
    QAction *actionGrid;
    QAction *actionDatawindow;
    QAction *actionNewIndicator;
    QAction *actionPortfolio;
    QAction *actionQuotes;
    QAction *actionScaleToScreen;
    QAction *actionNewPlugin;
    QAction *actionWorkwithTest;

    QToolBar *toolbar;
    QMenuBar *menubar;
    QSplitter *split;
    QTabWidget *tabs;
    QWidget *baseWidget;
    Navigator *chartNav;
    Navigator *groupNav;
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
    QLabel *infoLabel;
};

#endif 

