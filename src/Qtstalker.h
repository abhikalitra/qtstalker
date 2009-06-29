/***************************************************************************
                          qtstalker.h  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2007 by Stefan Stratigakos
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
#include "IndicatorPlugin.h"
#include "Plot.h"
#include "Setting.h"
#include "ChartPage.h"
#include "IndicatorPage.h"
#include "ScannerPage.h"
#include "TestPage.h"
#include "GroupPage.h"



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
    void signalInterval(BarData::BarLength);
    void signalChartPath (QString);
    void signalCrosshairsStatus(bool);
    void signalClearIndicator ();
    void signalGrid (bool);
    void signalScale (bool);
    void signalDraw (bool);
    void signalNewIndicator ();
    void signalCrossHair (int, int, bool);

  public:

    enum chartStatus
    {
      None,
      Chart
    };

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
      DrawMode,
      Crosshairs,
      Help,
      IndicatorSummary,
      Compression,
      CompressionM,
      CompressionW,
      CompressionD,
      Compression60,
      Compression15,
      Compression5,
      PixelSpace,
      PS1,
      PS2,
      PS3,
      BarCount,
      Slider
    };

    QtstalkerApp ();
    ~QtstalkerApp ();
    void initConfig ();
    void createActions ();
    void createMenuBar ();
    void createToolBars ();
    void initGroupNav ();
    void initChartNav ();
    void initTestNav();
    void initIndicatorNav ();
    void initScannerNav ();
    QString getWindowCaption ();
    void loadChart (QString);
    void setSliderStart ();

  public slots:
    void slotAbout ();
    void slotQuit();
    void slotOpenChart (QString);
    void slotOptions ();
    void slotDataWindow ();
    void slotNewIndicator (QString);
    void slotDeleteIndicator (QString);
    void slotBarLengthChanged (int);
    void slotPixelspaceChanged (int);
    void slotChartUpdated ();
    void slotStatusMessage (QString);
    void slotUpdateInfo (Setting *);
    void slotPlotLeftMouseButton (int, int, bool);
    void slotCrosshairsStatus (bool);
    void slotDisableIndicator (QString);
    void slotEnableIndicator (QString);
    void slotDrawPlots ();
    void addIndicatorButton (QString);
    void slotWakeup ();
    void slotIndicatorSummary ();
    void slotMenubarStatus (bool);
//    void slotExtraToolbarStatus (bool);
    void slotAppFont (QFont);
    void slotHideNav (bool d);
    void ps1ButtonClicked ();
    void ps2ButtonClicked ();
    void ps3ButtonClicked ();
    void cmpsBtnMClicked();
    void cmpsBtnWClicked();
    void cmpsBtnDClicked();
    void cmpsBtn60Clicked();
    void cmpsBtn15Clicked();
    void cmpsBtn5Clicked();
    int setSliderStart (int width, int records);
    void setPixelspace (int min, int d);
    void slotOrientationChanged(Qt::Orientation o);
    void slotEditIndicator (QString);
    
  private:
    QSplitter *split;
    QSplitter *navSplitter;
    QSplitter *dpSplitter;
    QTabWidget *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    QHash<QString, Plot *> plotList;
    chartStatus status;
    QString chartPath;
    QString chartName;
    QString chartSymbol;
    BarData *recordList;
    QTextEdit *infoLabel;
    IndicatorPage *ip;
    ScannerPage *sp;
    TestPage *tp;
    GroupPage *gp;
    QStatusBar *statusbar;
    QString chartType;
    QList<QTabWidget*> tabList;
    QString lastIndicatorUsed1;
    QString lastIndicatorUsed2;
    QString lastIndicatorUsed3;
    QHash<MenuAction, QAction*> actionList;
    QComboBox *compressionCombo;
    QSpinBox *pixelspace;
    QSpinBox *barCount;
    QSlider *slider;
};

#endif

