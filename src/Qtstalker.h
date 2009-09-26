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
#include <QSpinBox>


#include "Plot.h"
#include "Setting.h"
#include "ChartPage.h"
#include "IndicatorPage.h"
#include "GroupPage.h"
#include "ExScript.h"



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
      PS1,
      PS2,
      BarCount,
      Slider,
      RecentCharts,
      ZoomIn,
      ZoomOut
    };

    QtstalkerApp (QString);
    void initConfig ();
    void createActions ();
    void createMenuBar ();
    void createToolBars ();
    void initGroupNav ();
    void initChartNav ();
    void initIndicatorNav ();
//    void initTestNav();
//    void initScannerNav ();
    QString getWindowCaption ();
    void loadChart (QString);
    void setSliderStart ();

  public slots:
    void slotAbout ();
    void slotQuit();
    void slotOpenChart (QString);
    void slotOpenChart (int);
    void slotOptions ();
    void slotDataWindow ();
    void slotNewIndicator (QString);
    void slotDeleteIndicator (QString);
    void slotBarLengthChanged (int);
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
    void slotAppFont (QFont);
    void slotHideNav (bool d);
    void ps1ButtonClicked ();
    void ps2ButtonClicked ();
    void cmpsBtnMClicked();
    void cmpsBtnWClicked();
    void cmpsBtnDClicked();
    void cmpsBtn60Clicked();
    void cmpsBtn15Clicked();
    void cmpsBtn5Clicked();
    void slotAddRecentChart (QString);
    void slotScriptDone ();

    void slotZoomIn ();
    void slotZoomOut ();
    void slotPlotZoom (int, int);
    void resetZoomSettings ();
    
  private:
    QSplitter *split;
    QSplitter *navSplitter;
    QSplitter *dpSplitter;
    QTabWidget *navTab;
    QWidget *baseWidget;
    QWidget *navBase;
    ChartPage *chartNav;
    QHash<QString, Plot *> plotList;
    QString chartPath;
    BarData *recordList;
    QTextEdit *infoLabel;
    IndicatorPage *ip;
//    ScannerPage *sp;
//    TestPage *tp;
    GroupPage *gp;
    QStatusBar *statusbar;
    QList<QTabWidget*> tabList;
    QStringList lastIndicatorUsed;
    QHash<MenuAction, QAction*> actionList;
    QComboBox *compressionCombo;
    QSpinBox *barCount;
    QSlider *slider;
    QComboBox *recentCharts;
    
    ExScript *script;
    QStringList indicatorList;
    int ilPos;

    QList<Setting> zoomList;
    int zoomPos;
};

#endif

