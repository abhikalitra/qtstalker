/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2008 by Stefan Stratigakos
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

#include <QLayout>
#include <QMessageBox>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QFont>
#include <QCursor>
#include <QColor>
#include <QStringList>
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <math.h> // only for fabs()
#include <QtDebug>
#include <QToolButton>
#include <QApplication>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
//#include "IndicatorSummary.h"
#include "Preferences.h"
#include "DataBase.h"
#include "Indicator.h"
#include "Setup.h"
#include "Config.h"

//#include "../lib/qtstalker_defines.h"

#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/test.xpm"
#include "../pics/scanner.xpm"
#include "../pics/done.xpm"
#include "../pics/grid.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/configure.xpm"
#include "../pics/scaletoscreen.xpm"
#include "../pics/nav.xpm"
#include "../pics/co.xpm"
#include "../pics/qtstalker.xpm"
#include "../pics/crosshair.xpm"
//#include "../pics/include.xpm"



QtstalkerApp::QtstalkerApp(QString session)
{
  recordList = 0;
  status = None;
  setWindowIcon(QIcon(qtstalker));

  // setup the disk environment
  Setup setup;
  setup.setup(session);

  createActions();
  createMenuBar();
  createToolBars();
  
  statusbar = statusBar();

  // slider
  slider = new QSlider;
  slider->setOrientation(Qt::Horizontal);
  slider->setEnabled(FALSE);
  slider->setToolTip(tr("Pan Chart"));
  slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
//  action = toolbar2->addWidget(slider);
//  actionList.insert(Slider, action);
//  config.getData(Config::ShowSlider, ts);
//  action->setVisible(ts.toInt());
  statusbar->addPermanentWidget(slider, 0);
  
  baseWidget = new QWidget;
  setCentralWidget (baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  baseWidget->setLayout(hbox);

  navSplitter = new QSplitter;
  navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(navSplitter);

  // construct the chart areas
  QWidget *chartBase = new QWidget;
  navSplitter->addWidget(chartBase);
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  chartBase->setLayout(vbox);

  split = new QSplitter;
  split->setOrientation(Qt::Vertical);
  vbox->addWidget(split);

  // build the tab rows
  int loop;
  for (loop = 0; loop < 3; loop++)
  {
    QTabWidget *it = new QTabWidget;
    split->addWidget(it);
    it->setContentsMargins(0, 0, 0, 0);
    connect(it, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotDrawPlots()));
    tabList.append(it);
  }

  navBase = new QWidget;
  navSplitter->addWidget(navBase);
  vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  navBase->setLayout(vbox);
  
  // setup the data panel splitter
  dpSplitter = new QSplitter;
  dpSplitter->setOrientation(Qt::Vertical);
  vbox->addWidget(dpSplitter);
 
  // setup the side panels
  navTab = new QTabWidget;
  dpSplitter->addWidget(navTab);
//  connect(extraToolbar, SIGNAL(recentTab(QString)), navTab, SLOT(recentTab(QString)));
  
  // setup the data panel area
  infoLabel = new QTextEdit;
  dpSplitter->addWidget(infoLabel);
  infoLabel->setReadOnly(TRUE);

  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initTestNav();
  initScannerNav();  

  // restore the last used indicators
  QString s;
  Config config;
  config.getData(Config::LastIndicatorUsed, s);
  QStringList l = s.split(",");
  if (l.count() == 1)
    lastIndicatorUsed1 = l[0];
  if (l.count() == 2)
  {
    lastIndicatorUsed1 = l[0];
    lastIndicatorUsed2 = l[1];
  }
  if (l.count() == 3)
  {
    lastIndicatorUsed1 = l[0];
    lastIndicatorUsed2 = l[1];
    lastIndicatorUsed3 = l[2];
  }

  // setup the initial indicators
  DataBase db;
  db.getIndicatorList(l);
  for (loop = 0; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    db.getIndicator(i);
    if (i.getEnable())
      addIndicatorButton(l[loop]);
  }

  // set the app font
  QFont font;
  config.getData(Config::AppFont, font);
  slotAppFont(font);

  // restore the splitter sizes
  config.getData(Config::PlotSizes, split);
  config.getData(Config::NavAreaSize, navSplitter);
  config.getData(Config::DataPanelSize, dpSplitter);
  
  // restore the size of the app
  QSize sz;
  config.getData(Config::MainWindowSize, sz);
  resize(sz);
  
  // restore the position of the app
  QPoint p;
  config.getData(Config::MainWindowPos, p);
  move(p);
  
  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));
  
  statusbar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::createActions ()
{
  // create the actions needed for menu and toolbar

  QAction *action  = new QAction(QIcon(finished), tr("E&xit"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  action->setStatusTip(tr("Quit Qtstalker (Ctrl+Q)"));
  action->setToolTip(tr("Quit Qtstalker (Ctrl+Q)"));
  connect(action, SIGNAL(activated()), qApp, SLOT(quit()));
  actionList.insert(Exit, action);
  
  action = new QAction(QIcon(indicator), tr("New &Indicator"), this);
  action->setStatusTip(tr("Add a new indicator to chart (Ctrl+2)"));
  action->setToolTip(tr("Add a new indicator to chart (Ctrl+2)"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalNewIndicator()));
  actionList.insert(NewIndicator, action);

  action = new QAction(QIcon(configure), tr("Edit &Preferences"), this);
  action->setStatusTip(tr("Modify user preferences  (Ctrl+3)"));
  action->setToolTip(tr("Modify user preferences  (Ctrl+3)"));
  connect(action, SIGNAL(activated()), this, SLOT(slotOptions()));
  actionList.insert(Options, action);

  QString s;
  Config config;
  config.getData(Config::Grid, s);
  action = new QAction(QIcon(gridicon), tr("Chart &Grid"), this);
  action->setStatusTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setToolTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setCheckable(TRUE);
  action->setChecked(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalGrid(bool)));
  actionList.insert(Grid, action);

  action = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  action->setStatusTip(tr("Show the data window (Alt+1)"));
  action->setToolTip(tr("Show the data window (Alt+1)"));
  connect(action, SIGNAL(activated()), this, SLOT(slotDataWindow()));
  actionList.insert(DataWindow1, action);

  action = new QAction(QIcon(qtstalker), tr("&About"), this);
  action->setStatusTip(tr("About Qtstalker."));
  action->setToolTip(tr("About Qtstalker."));
  connect(action, SIGNAL(activated()), this, SLOT(slotAbout()));
  actionList.insert(About, action);

  config.getData(Config::ScaleToScreen, s);
  action = new QAction(QIcon(scaletoscreen), tr("&Scale To Screen"), this);
  action->setStatusTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setToolTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setCheckable(TRUE);
  action->setChecked(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actionList.insert(ScaleToScreen, action);

  config.getData(Config::ShowSidePanel, s);
  action = new QAction(QIcon(nav), tr("Side Pa&nel"), this);
  action->setStatusTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setToolTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setCheckable(TRUE);
  action->setChecked(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SLOT(slotHideNav(bool)));
  actionList.insert(SidePanel, action);

  config.getData(Config::DrawMode, s);
  action = new QAction(QIcon(co), tr("Toggle Dra&w Mode"), this);
  action->setStatusTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setToolTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setCheckable(TRUE);
  action->setChecked(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalDraw(bool)));
  actionList.insert(DrawMode, action);
  
  config.getData(Config::Crosshairs, s);
  action = new QAction(QIcon(crosshair), tr("Toggle Cross&hairs"), this);
  action->setStatusTip(tr("Toggle crosshairs (Ctrl+6)"));
  action->setToolTip(tr("Toggle crosshairs (Ctrl+6)"));
  action->setCheckable(TRUE);
  action->setChecked(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalCrosshairsStatus(bool)));
  actionList.insert(Crosshairs, action);

//  action = new QAction(QIcon("../pics/help.xpm"), tr("&Help"), this);
//  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_3));
//  action->setStatusTip(tr("Display Help Dialog (Alt+3)"));
//  action->setToolTip(tr("Display Help Dialog (Alt+3)"));
//  connect(action, SIGNAL(activated()), mw, SLOT(slotHelp()));
//  actionList.insert(Help, action);

//  action = new QAction(this);
//  action->setText(tr("Indicator Summary"));
//  action->setStatusTip(tr("Indicator Summary"));
//  action->setToolTip(tr("Indicator Summary"));
//  connect(action, SIGNAL(activated()), this, SLOT(slotIndicatorSummary()));
//  actionList.insert(IndicatorSummary, action);
}

void QtstalkerApp::createMenuBar()
{
  // create the main menubar
  QMenuBar *menubar = menuBar();

  //attach it...mainwindow takes ownership

  QMenu *menu = new QMenu();
  menu->setTitle(tr("&File"));
  menu->addAction(actionList.value(Exit));
  menubar->addMenu(menu);

  menu = new QMenu();
  menu->setTitle(tr("&Edit"));
  menu->addAction(actionList.value(NewIndicator));
  menu->addAction(actionList.value(Options));
  menubar->addMenu(menu);

  menu = new QMenu();
  menu->setTitle(tr("&View"));
  menu->addAction(actionList.value(Grid));
  menu->addAction(actionList.value(ScaleToScreen));
  menu->addAction(actionList.value(SidePanel));
  menu->addAction(actionList.value(DrawMode));
  menu->addAction(actionList.value(Crosshairs));
  menubar->addMenu(menu);

  menu = new QMenu();
  menu->setTitle(tr("&Tools"));
  menu->addAction(actionList.value(DataWindow1));
//  menu->addAction(actionList.value(IndicatorSummary));
  menubar->addMenu(menu);

  menubar->addSeparator();

  menu = new QMenu();
  menu->setTitle(tr("&Help"));
  menu->addAction(actionList.value(About));
  menu->addAction(actionList.value(Help));
  menubar->addMenu(menu);
}

void QtstalkerApp::createToolBars ()
{
  // construct the button toolbar
  QToolBar *toolbar = addToolBar("buttonToolbar");

  //construct main toolbar
  QString tb;
  Config config;
  config.getData(Config::ShowQuitBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(Exit));

  config.getData(Config::ShowPrefBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(Options));

  config.getData(Config::ShowSidePanelBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(SidePanel));

  config.getData(Config::ShowGridBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(Grid));

  config.getData(Config::ShowScaleToScreenBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(ScaleToScreen));

  config.getData(Config::ShowCrosshairBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(Crosshairs));

  config.getData(Config::ShowDrawModeBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(DrawMode));

  config.getData(Config::ShowNewIndicatorBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(NewIndicator));

  config.getData(Config::ShowDataWindowBtn, tb);
  if (tb.toInt())
    toolbar->addAction(actionList.value(DataWindow1));

  // construct the chart toolbar
  QToolBar *toolbar2 = addToolBar("chartToolBar");
  connect(toolbar2, SIGNAL(orientationChanged(Qt::Orientation)), this, SLOT(slotOrientationChanged(Qt::Orientation)));
  //setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
  //setVerticallyStretchable(TRUE);

  int minPixelspace = 2;
  QString ts; // temporary string
  BarData bd(ts);
  QStringList l;
  bd.getBarLengthList(l);

  // compression
  compressionCombo = new QComboBox;
  compressionCombo->addItems(l);
  config.getData(Config::BarLength, ts);
  compressionCombo->setCurrentIndex(ts.toInt());
  compressionCombo->setToolTip(tr("Bar Length (Compression)"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(slotBarLengthChanged(int)));
  QAction *action = toolbar2->addWidget(compressionCombo);
  actionList.insert(Compression, action);
  config.getData(Config::ShowCmpsComboBox, ts);
  action->setVisible(ts.toInt());

  // monthly compression button  
  QToolButton *b = new QToolButton; // compression button monthly
  b->setToolTip(tr("Monthly Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnMClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(CompressionM, action);
  config.getData(Config::ShowCmpsMtyBtn, ts);
  action->setVisible(ts.toInt());
  b->setText("M");

  // weekly compression button  
  b = new QToolButton; // compression button weekly
  b->setToolTip(tr("Weekly Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnWClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(CompressionW, action);
  config.getData(Config::ShowCmpsWkyBtn, ts);
  action->setVisible(ts.toInt());
  b->setText("W");

  // daily compression button  
  b = new QToolButton; //   button daily
  b->setToolTip(tr("Daily Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnDClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(CompressionD, action);
  config.getData(Config::ShowCmpsDayBtn, ts);
  action->setVisible(ts.toInt());
  b->setText("D");
  
  // 60 minute compression button  
  b = new QToolButton; // compression button 60min
  b->setToolTip(tr("60min Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn60Clicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(Compression60, action);
  config.getData(Config::ShowCmps60Btn, ts);
  action->setVisible(ts.toInt());
  b->setText("60");

  // 15 minute compression button  
  b = new QToolButton; // compression button 15min
  b->setToolTip(tr("15min Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn15Clicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(Compression15, action);
  config.getData(Config::ShowCmps15Btn, ts);
  action->setVisible(ts.toInt());
  b->setText("15");

  // 5 minute compression button  
  b = new QToolButton; // compression button 5min
  b->setToolTip(tr("5min Compression"));
//  b->setCheckable(TRUE);
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn5Clicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(Compression5, action);
  config.getData(Config::ShowCmps5Btn, ts);
  action->setVisible(ts.toInt());
  b->setText("5");

  toolbar2->addSeparator();

  // pixelspace 
  pixelspace = new QSpinBox;
  pixelspace->setRange(minPixelspace, 99);
  config.getData(Config::Pixelspace, ts);
  pixelspace->setValue(ts.toInt());
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  pixelspace->setToolTip(tr("Bar Spacing"));
  action = toolbar2->addWidget(pixelspace);
  actionList.insert(PixelSpace, action);
  config.getData(Config::ShowBarSpSpinbox, ts);
  action->setVisible(ts.toInt());
  pixelspace->setMaximumWidth(40); // FIXME:calc as a function of app font metrics
  
  // PS1 button  
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(ps1ButtonClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(PS1, action);
  config.getData(Config::PSButton1, ts);
  b->setToolTip(tr("Set Bar Spacing to ") + ts);
  b->setText(ts);
  
  // PS2 button  
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(ps2ButtonClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(PS2, action);
  config.getData(Config::PSButton2, ts);
  b->setToolTip(tr("Set Bar Spacing to ") + ts);
  b->setText(ts);
  
  // PS3 button  
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(ps3ButtonClicked()));
  action = toolbar2->addWidget(b);
  actionList.insert(PS3, action);
  config.getData(Config::PSButton3, ts);
  b->setToolTip(tr("Set Bar Spacing to ") + ts);
  b->setText(ts);
  
  toolbar2->addSeparator();

  //bars to load  
  barCount = new QSpinBox;
  barCount->setRange(1, 9999);
  config.getData(Config::BarsToLoad, ts);
  barCount->setValue(ts.toInt());
  barCount->setToolTip(tr("Total bars to load"));
  connect(barCount, SIGNAL(valueChanged(int)), this, SLOT(slotChartUpdated()));
  action = toolbar2->addWidget(barCount);
  actionList.insert(BarCount, action);
  config.getData(Config::ShowBarsToLoadField, ts);
  action->setVisible(ts.toInt());
//  barCount->setMaximumWidth(40);// FIXME:calc as a function of app font metrics
  
  toolbar2->addSeparator();

  // construct the extra toolbar
//  QToolBar *extraToolbar = addToolBar("extraToolBar");
  //attach to qmainwindwo...
  //remove for testing!
//  addToolBar(extraToolbar);
//  extraToolbar->slotSetButtonView();
//  connect(extraToolbar, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
}

void QtstalkerApp::slotQuit()
{
  Config config;

  // do this to save any pending chart object edits
  emit signalClearIndicator();

  // save last indicators used
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    QString s = tw->tabText(tw->currentIndex());
    l.append(s);
  }
  QString s = l.join(",");
  config.setData(Config::LastIndicatorUsed, s);

  // save window sizes 
  config.setData(Config::PlotSizes, split);
  config.setData(Config::DataPanelSize, dpSplitter);
  config.setData(Config::NavAreaSize, navSplitter);
   
  // save app size and position
  QSize sz = size();
  config.setData(Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setData(Config::MainWindowPos, pt);

  // save menubar settings
  s = QString::number(actionList.value(DrawMode)->isChecked());
  config.setData(Config::DrawMode, s);

  s = QString::number(actionList.value(ScaleToScreen)->isChecked());
  config.setData(Config::ScaleToScreen, s);

  s = QString::number(actionList.value(Grid)->isChecked());
  config.setData(Config::Grid, s);

  s = QString::number(actionList.value(Crosshairs)->isChecked());
  config.setData(Config::Crosshairs, s);

  s = QString::number(actionList.value(SidePanel)->isChecked());
  config.setData(Config::ShowSidePanel, s);

  // save toolbar settings
  QString ts(barCount->text()); 
  config.setData(Config::BarsToLoad, ts);

  ts = QString::number(compressionCombo->currentIndex());
  config.setData(Config::BarLength, ts);

  ts = pixelspace->text();
  config.setData(Config::Pixelspace, ts);
  //rcfile.savePoint(RcFile::ChartToolBarPos, pos());
  //FIXME: IMPORTANT- save all belonged to restore the position of the toolbar. maybe not here but in Qtstalker.cpp. Possible is this the way
  //http://doc.trolltech.com/3.3/qdockarea.html#operator-lt-lt

  // delete any BarData
  if (recordList)
    delete recordList;

  // call the destructors which save some settings
  delete gp;
  delete chartNav;

  qDeleteAll(plotList);
  qDeleteAll(tabList);
  qDeleteAll(actionList);
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  QString versionString = "Qtstalker\nVersion 0.37-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2001-2008 by Stefan Stratigakos\nqtstalker.sourceforge.net";
  QMessageBox::about(this, tr("About Qtstalker"), versionString);
}

void QtstalkerApp::slotOpenChart (QString selection)
{
  // load a chart slot
  slider->setEnabled(TRUE);
  status = Chart;
  qApp->processEvents();
  loadChart(selection);
}

void QtstalkerApp::slotOptions ()
{
  Preferences *dialog = new Preferences(this);
  connect(dialog, SIGNAL(signalMenubar(bool)), this, SLOT(slotMenubarStatus(bool)));
//  connect(dialog, SIGNAL(signalExtraToolbar(bool)), this, SLOT(slotExtraToolbarStatus(bool)));
  connect(dialog, SIGNAL(signalBackgroundColor(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));
  connect(dialog, SIGNAL(signalBorderColor(QColor)), this, SIGNAL(signalBorderColor(QColor)));
  connect(dialog, SIGNAL(signalGridColor(QColor)), this, SIGNAL(signalGridColor(QColor)));
  connect(dialog, SIGNAL(signalPlotFont(QFont)), this, SIGNAL(signalPlotFont(QFont)));
  connect(dialog, SIGNAL(signalAppFont(QFont)), this, SLOT(slotAppFont(QFont)));
  connect(dialog, SIGNAL(signalLoadChart()), this, SLOT(slotChartUpdated()));
//  connect(dialog, SIGNAL(signalReloadToolBars()), this, SLOT(slotLoadMainToolbarSettings()));
  
  dialog->show();
}

void QtstalkerApp::loadChart (QString d)
{
  // do all the stuff we need to do to load a chart
  if (d.length() == 0)
    return;

  // check if we need to save the slider position because chart is reloaded
  bool reload = FALSE;
  if (! chartPath.compare(d))
    reload = TRUE;

  chartPath = d;

  // set plots to empty
  emit signalClearIndicator();

  // update each plot with new symbol
  emit signalChartPath(chartPath);

  // save the new chart
  Config config;
  config.setData(Config::CurrentChart, chartPath);

  if (recordList)
    delete recordList;
  recordList = new BarData(chartPath);
  recordList->setBarLength((BarData::BarLength) compressionCombo->currentIndex());
  recordList->setBarsRequested(barCount->value());

  slotStatusMessage(tr("Loading chart..."));

  DataBase db;
  db.getChart(recordList);

  chartSymbol = chartPath;
  recordList->getType(chartType);

  QStringList l;
  db.getIndicatorList(l);
  int loop;  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Plot *plot = plotList[l[loop]];
    if (! plot)
      continue;
    plot->setData(recordList);
    plot->calculate();
    plot->loadChartObjects();
  }
  
  setSliderStart();

  slotDrawPlots();

  setWindowTitle(getWindowCaption());
  
  slotStatusMessage(QString());
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text
  
  QString caption = tr("Qtstalker");

  switch (status)
  {
    case Chart:
      caption.append(": ");
      caption.append(chartName);
      caption.append(" (");
      if (chartSymbol.length() > 0)
        caption.append(chartSymbol);
      else
        caption.append(chartType);
      caption.append(") ");
      break;
    default:
      break;
  }

  caption.append(compressionCombo->currentText());

  return caption;
}

void QtstalkerApp::slotDataWindow ()
{
  // show the datawindow dialog
  
  if (! recordList)
    return;

  DataWindow *dw = new DataWindow(this);
  dw->setWindowTitle(getWindowCaption());
  dw->setBars(recordList);
  
  DataBase db;
  QStringList l;
  db.getIndicatorList(l);
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    Plot *plot = plotList.value(l[loop]);
    if (! plot)
      continue;

    dw->setPlot(plot);
  }
  
  dw->show();
}

void QtstalkerApp::slotBarLengthChanged(int barLength)
{
  QString s;
  Config config;
  config.getData(Config::BarLength, s);
  int n = s.toInt();
  if (n == barLength)
    return;
 
  // the compression has changed
  s = QString::number(compressionCombo->currentIndex());
  config.setData(Config::BarLength, s);

  barLength == 8 ? actionList.value(CompressionM)->setChecked(TRUE) : actionList.value(CompressionM)->setChecked(FALSE);
  barLength == 7 ? actionList.value(CompressionW)->setChecked(TRUE) : actionList.value(CompressionW)->setChecked(FALSE);
  barLength == 6 ? actionList.value(CompressionD)->setChecked(TRUE) : actionList.value(CompressionD)->setChecked(FALSE);
  barLength == 5 ? actionList.value(Compression60)->setChecked(TRUE) : actionList.value(Compression60)->setChecked(FALSE);
  barLength == 3 ? actionList.value(Compression15)->setChecked(TRUE) : actionList.value(Compression15)->setChecked(FALSE);
  barLength == 1 ? actionList.value(Compression5)->setChecked(TRUE) : actionList.value(Compression5)->setChecked(FALSE);

  emit signalInterval((BarData::BarLength) compressionCombo->currentIndex());

  loadChart(chartPath);
}

void QtstalkerApp::slotNewIndicator (QString n)
{
  // add a new indicator slot
  addIndicatorButton(n);
  Plot *p = plotList.value(n);
  if (! p)
    return;
  if (! recordList)
    return;
  p->setData(recordList);
  p->calculate();
  p->draw();
}

void QtstalkerApp::slotDeleteIndicator (QString text)
{
  // delete indicator slot
  int loop;
  for (loop = 0; loop < tabList.count(); loop++)
  {
    QTabWidget *it = tabList.at(loop);
    int loop2;
    for (loop2 = 0; loop2 < it->count(); loop2++)
    {
      if (QString::compare(it->tabText(loop2), text) == 0)
      {
        it->removeTab(loop2);
        break;
      }
    }
  }

  Plot *plot = plotList.value(text);
  if (plot)
    delete plot;
  plotList.remove(text);
}

void QtstalkerApp::slotDisableIndicator (QString name)
{
  slotDeleteIndicator(name);
}

void QtstalkerApp::slotEnableIndicator (QString name)
{
  // enable indicator
  addIndicatorButton(name);
  Plot *p = plotList.value(name);
  if (! p)
    return;
  if (! recordList)
    return;
  p->setData(recordList);
  p->calculate();
  p->draw();
}

void QtstalkerApp::slotEditIndicator (QString name)
{
  Plot *p = plotList.value(name);
  if (! p)
    return;

  if (! recordList)
    return;

  DataBase db;
  Indicator i;
  i.setName(name);
  db.getIndicator(i);
  p->getIndicatorPlot()->setIndicator(i);

  p->calculate();
  p->draw();
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  emit signalPixelspace(d);
  emit signalIndex(slider->value());
  setSliderStart();
  slotDrawPlots();
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  DataBase db;
  QString s;
  Indicator i;
  i.setName(d);
  db.getIndicator(i);
  if (! i.getEnable())
    return;

  Plot *plot = new Plot(baseWidget);
  plotList.insert(d, plot);
  plot->setDateFlag(i.getDate());
  plot->setLogScale(i.getLog());

  QTabWidget *it = tabList.at(i.getTabRow() - 1);
  it->addTab(plot, d);

  // Set the current indicator in this row to the last used one.
  switch (i.getTabRow())
  {
    case 1:
      if (d == lastIndicatorUsed1)
        it->setCurrentWidget(plot);
      break;
    case 2:
      if (d == lastIndicatorUsed2)
        it->setCurrentWidget(plot);
      break;
    case 3:
      if (d == lastIndicatorUsed3)
        it->setCurrentWidget(plot);
      break;
    default:
      break;
  }

  QColor color;
  Config config;
  config.getData(Config::BackgroundColor, color);
  
  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  config.getData(Config::BorderColor, color);
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  config.getData(Config::GridColor, color);
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QFont font;
  config.getData(Config::PlotFont, font);
  plot->setPlotFont(font);

  plot->setGridFlag(actionList.value(Grid)->isChecked());
  plot->setScaleToScreen(actionList.value(ScaleToScreen)->isChecked());
  plot->setPixelspace(pixelspace->value());
  plot->setIndex(slider->value());
  plot->setInterval((BarData::BarLength) compressionCombo->currentIndex());
  QString b;
  config.getData(Config::Crosshairs, b);
  plot->setCrosshairsStatus(b.toInt());  
  plot->setDrawMode(actionList.value(DrawMode)->isChecked());

  IndicatorPlot *indy = plot->getIndicatorPlot();
  indy->setIndicator(i);
    
  connect(indy, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(indy, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  connect(indy, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  connect(this, SIGNAL(signalCrossHair(int, int, bool)), indy, SLOT(crossHair(int, int, bool)));
  connect(this, SIGNAL(signalCrosshairsStatus(bool)), indy, SLOT(setCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
  connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));
  connect(this, SIGNAL(signalClearIndicator()), plot, SLOT(clear()));
  connect (slider, SIGNAL(valueChanged(int)), plot, SLOT(slotSliderChanged(int)));
  connect(this, SIGNAL(signalGrid(bool)), indy, SLOT(slotGridChanged(bool)));
  connect(this, SIGNAL(signalScale(bool)), plot, SLOT(slotScaleToScreenChanged(bool)));
  connect(this, SIGNAL(signalDraw(bool)), indy, SLOT(slotDrawModeChanged(bool)));
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->refreshList();
  
  if (status == None)
    return;
  
  Config config;
  QString s(barCount->value());
  config.setData(Config::BarsToLoad, s);

  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusbar->showMessage(d);
  qApp->processEvents();
}

void QtstalkerApp::initChartNav ()
{
  chartNav = new ChartPage(baseWidget);
  connect(chartNav, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
//  connect(chartNav, SIGNAL(addRecentChart(QString)), extraToolbar, SLOT(slotAddRecentChart(QString)));
//  connect(chartNav, SIGNAL(removeRecentCharts(QStringList)), extraToolbar, SLOT(slotRemoveRecentCharts(QStringList)));
  connect(chartNav, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  navTab->addTab(chartNav, QIcon(plainitem), QString());
  navTab->setTabToolTip(0, tr("Charts"));
}

void QtstalkerApp::initGroupNav ()
{
  gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
//  connect(chartNav, SIGNAL(signalAddToGroup(QString)), gp, SLOT(addChartToGroup(QString)));
//  connect(gp, SIGNAL(addRecentChart(QString)), extraToolbar, SLOT(slotAddRecentChart(QString)));
//  connect(gp, SIGNAL(removeRecentCharts(QStringList)), extraToolbar, SLOT(slotRemoveRecentCharts(QStringList)));
//  connect(extraToolbar, SIGNAL(signalSetGroupNavItem(QString, QString)), gp, SLOT(setGroupNavItem(QString, QString)));
  navTab->addTab(gp, QIcon(dirclosed), QString());
  navTab->setTabToolTip(1, tr("Groups"));
}

void QtstalkerApp::initTestNav ()
{
  tp = new TestPage(baseWidget);
  connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addTab(tp, QIcon(test), QString());
  navTab->setTabToolTip(4, tr("Backtesting"));
}

void QtstalkerApp::initIndicatorNav ()
{
  ip = new IndicatorPage(baseWidget);
  connect(ip, SIGNAL(signalDisableIndicator(QString)), this, SLOT(slotDisableIndicator(QString)));
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(slotEnableIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(QString)), this, SLOT(slotNewIndicator(QString)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  connect(this, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalEditIndicator(QString)), this, SLOT(slotEditIndicator(QString)));
  navTab->addTab(ip, QIcon(indicator), QString());
  navTab->setTabToolTip(2, tr("Indicators"));
}

void QtstalkerApp::initScannerNav ()
{
  sp = new ScannerPage(baseWidget);
  connect(sp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(sp, SIGNAL(refreshGroup()), gp, SLOT(updateGroups()));
  navTab->addTab(sp, QIcon(scanner), QString());
  navTab->setTabToolTip(5, tr("Scanners"));
}

void QtstalkerApp::slotHideNav (bool d)
{
  if (d)
    navBase->show();
  else
    navBase->hide();
}

void QtstalkerApp::slotUpdateInfo (Setting *r)
{
  // list bar values first
  QStringList l;
  l.append("D");
  l.append("T");
  l.append("O");
  l.append("H");
  l.append("L");
  l.append("C");
  l.append("VOL");
  l.append("OI");
  QString s, s2, str;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    r->getData(l[loop], s);
    if (s.length())
    {
      str.append(l[loop] + " " + s + "\n");
      r->remove(l[loop]);
    }
  }

  r->getKeyList(l);
  l.sort();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    r->getData(l[loop], s);
    // If it is a big number, then use zero precision.
    bool ok;
    double sn = s.toDouble(&ok);
    if (ok) {
      if (fabs(sn) > 1000)
        s = QString::number(sn, 'f', 0);
    }
    str.append(l[loop] + " " + s + "\n");
  }

  delete r;

  infoLabel->setText(str);
}

void QtstalkerApp::slotPlotLeftMouseButton (int x, int y, bool)
{
  emit signalCrossHair(x, y, FALSE);
/*
  QHashIterator<QString, Plot *> it(plotList);
  while (it.hasNext())
  {
    it.next();
    it.value()->crossHair(x, y, FALSE);
  }
*/
  slotDrawPlots();  
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
}

void QtstalkerApp::setSliderStart ()
{
  if (status == None)
    return;

  int loop;
  int rc = 0;
  for (loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    if (! tw->isHidden())
    {
      Plot *plot = plotList[tw->tabText(tw->currentIndex())];
      if (! plot)
        return;
      else
      {
        rc = setSliderStart(plot->getWidth(), recordList->count());
        break;
      }
    }
  }

  emit signalIndex(rc);
}

void QtstalkerApp::slotDrawPlots ()
{
  int loop;
  for(loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *it = tabList.at(loop);
    if (it->isHidden())
      continue;
    QString s = it->tabText(it->currentIndex());
    Plot *p = plotList[s];
    if (p)
      p->draw();
  }
}

void QtstalkerApp::slotWakeup ()
{
  qApp->processEvents();
}

void QtstalkerApp::slotIndicatorSummary ()
{
  if (status == None)
    return;

  QString basePath, s;
  Config config;
//  config.getData(Config::IndicatorPath, basePath);
//  config.getData(Config::IndicatorGroup, s);
  basePath.append("/" + s);

  QStringList l;
  QHashIterator<QString, Plot *> it(plotList);
  while (it.hasNext())
  {
    it.next();
//    IndicatorPlot *i = it.value()->getIndicatorPlot();
//    i->getName(s);
    l.append(basePath + "/" + s);
  }

//  IndicatorSummary is(l, toolbar2->getBars(), (BarData::BarLength) toolbar2->getBarLengthInt(), chartIndex);
//  connect(&is, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));
//  is.run();
}

void QtstalkerApp::slotMenubarStatus (bool d)
{
  if (d)
    menuBar()->show();
  else
    menuBar()->hide();
}

/*
void QtstalkerApp::slotExtraToolbarStatus (bool d)
{
  if (d)
    extraToolbar->show();
  else
    extraToolbar->hide();
}
*/

void QtstalkerApp::slotAppFont (QFont d)
{
  qApp->setFont(d, 0);
  qApp->setFont(d, "QComboBox");
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setPixelspace (int min, int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(min, 99);
  pixelspace->setValue(d);
  pixelspace->blockSignals(FALSE);
}

int QtstalkerApp::setSliderStart (int width, int records)
{
  int page = width / pixelspace->value();
  int max = records - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, records - 1);
  int rc = 0;
  
  if ((int) records < page)
    slider->setValue(0);
  else
  {
    slider->setValue(max + 1);
    rc = max + 1;
  }
  slider->blockSignals(FALSE);
  
  return rc;
}

void QtstalkerApp::ps1ButtonClicked ()
{
  QString ts;
  Config config;
  config.getData(Config::PSButton1, ts);
  pixelspace->setValue(ts.toInt());
}

void QtstalkerApp::ps2ButtonClicked ()
{
  QString ts;
  Config config;
  config.getData(Config::PSButton2, ts);
  pixelspace->setValue(ts.toInt());
}

void QtstalkerApp::ps3ButtonClicked ()
{
  QString ts;
  Config config;
  config.getData(Config::PSButton3, ts);
  pixelspace->setValue(ts.toInt());
}

void QtstalkerApp::cmpsBtnMClicked()
{
  compressionCombo->setCurrentIndex((int) BarData::MonthlyBar);
  slotBarLengthChanged((int) BarData::MonthlyBar);
}

void QtstalkerApp::cmpsBtnWClicked()
{
  compressionCombo->setCurrentIndex((int) BarData::WeeklyBar);
  slotBarLengthChanged((int) BarData::WeeklyBar);
}

void QtstalkerApp::cmpsBtnDClicked()
{
  compressionCombo->setCurrentIndex((int) BarData::DailyBar);
  slotBarLengthChanged((int) BarData::DailyBar);
}

void QtstalkerApp::cmpsBtn60Clicked()
{
  compressionCombo->setCurrentIndex((int) BarData::Minute60);
  slotBarLengthChanged((int) BarData::Minute60);
}

void QtstalkerApp::cmpsBtn15Clicked()
{
  compressionCombo->setCurrentIndex((int) BarData::Minute15);
  slotBarLengthChanged((int) BarData::Minute15);
}

void QtstalkerApp::cmpsBtn5Clicked()
{
  compressionCombo->setCurrentIndex((int) BarData::Minute5);
  slotBarLengthChanged((int) BarData::Minute5);
}

void QtstalkerApp::slotOrientationChanged(Qt::Orientation o)
{
  slider->setOrientation(o);
// updateGeometry();
}

