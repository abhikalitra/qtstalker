/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2009 by Stefan Stratigakos
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
#include <QDesktopServices>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "Preferences.h"
#include "DataBase.h"
#include "Indicator.h"
#include "Setup.h"
#include "Config.h"
#include "IndicatorPage.h"
#include "GroupPage.h"
#include "IndicatorFactory.h"
#include "IndicatorBase.h"

#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/done.xpm"
#include "../pics/grid.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/configure.xpm"
#include "../pics/scaletoscreen.xpm"
#include "../pics/nav.xpm"
#include "../pics/co.xpm"
#include "../pics/qtstalker.xpm"
#include "../pics/help.xpm"
#include "../pics/crosshair.xpm"
#include "../pics/zoomin.xpm"
#include "../pics/zoomout.xpm"
#include "../pics/refresh.xpm"
//#include "../pics/script.xpm"


QtstalkerApp::QtstalkerApp(QString session)
{
  recordList = 0;
  refreshTimer = 0;
  zoomPos = -1;
  setWindowIcon(QIcon(qtstalker));
  QApplication::setOrganizationName("Qtstalker");

  // setup the disk environment and init databases
  Setup setup;
  setup.setupDirectories();
  DataBase db(session);

  setup.setup();

  assistant = new Assistant;

  createActions();
  createMenuBar();
  createToolBars();

  statusbar = statusBar();

  // slider
  slider = new QSlider;
  slider->setOrientation(Qt::Horizontal);
  slider->setEnabled(FALSE);
  slider->setToolTip(tr("Scroll Chart"));
  slider->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
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
  Config config;
  int tabRows = config.getInt(Config::IndicatorTabRows);
  int loop;
  for (loop = 0; loop < tabRows; loop++)
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

  // setup the data panel area
  infoLabel = new QTextEdit;
  dpSplitter->addWidget(infoLabel);
  infoLabel->setReadOnly(TRUE);

  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();

  // setup the script server
//  script = new ExScript;
//  connect(script, SIGNAL(signalDone()), this, SLOT(slotScriptDone()));

  // setup the initial indicators
  QStringList l;
  db.getActiveIndicatorList(l);
  for (loop = 0; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    db.getIndicator(i);

    if (i.getTabRow() > tabList.count())
      continue;

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

  Config config;
  action = new QAction(QIcon(gridicon), tr("Chart &Grid"), this);
  action->setStatusTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setToolTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::Grid));
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

  action = new QAction(QIcon(help), tr("&Help"), this);
  action->setStatusTip(tr("Show documentation."));
  action->setToolTip(tr("Show documentation."));
  connect(action, SIGNAL(triggered()), this, SLOT(slotStartDocumentation()));
  actionList.insert(Help, action);

  action = new QAction(QIcon(scaletoscreen), tr("&Scale To Screen"), this);
  action->setStatusTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setToolTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::ScaleToScreen));
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actionList.insert(ScaleToScreen, action);

  action = new QAction(QIcon(nav), tr("Side Pa&nel"), this);
  action->setStatusTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setToolTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::ShowSidePanel));
  connect(action, SIGNAL(toggled(bool)), this, SLOT(slotHideNav(bool)));
  actionList.insert(SidePanel, action);

  action = new QAction(QIcon(co), tr("Toggle Dra&w Mode"), this);
  action->setStatusTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setToolTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::DrawMode));
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalDraw(bool)));
  actionList.insert(DrawMode, action);

  action = new QAction(QIcon(crosshair), tr("Toggle Cross&hairs"), this);
  action->setStatusTip(tr("Toggle crosshairs (Ctrl+6)"));
  action->setToolTip(tr("Toggle crosshairs (Ctrl+6)"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::Crosshairs));
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalCrosshairsStatus(bool)));
  actionList.insert(Crosshairs, action);

  action = new QAction(QIcon(refresh_xpm), tr("&Refresh Chart"), this);
  action->setStatusTip(tr("Refresh chart every x minutes"));
  action->setToolTip(tr("Refresh chart every x minutes"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::RefreshStatus));
  connect(action, SIGNAL(toggled(bool)), this, SLOT(slotRefreshChart(bool)));
  actionList.insert(Refresh, action);

//  action = new QAction(QIcon(script_xpm), tr("Run S&cript"), this);
//  action->setStatusTip(tr("Run Qtstalker script"));
//  action->setToolTip(tr("Run Qtstalker script"));
//  connect(action, SIGNAL(activated()), this, SLOT(slotScript()));
//  actionList.insert(Script, action);
}

void QtstalkerApp::createMenuBar()
{
  // create the main menubar
  QMenuBar *menubar = menuBar();

  QMenu *menu = new QMenu;
  menu->setTitle(tr("&File"));
  menu->addAction(actionList.value(Exit));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Edit"));
  menu->addAction(actionList.value(NewIndicator));
  menu->addAction(actionList.value(Options));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&View"));
  menu->addAction(actionList.value(Grid));
  menu->addAction(actionList.value(ScaleToScreen));
  menu->addAction(actionList.value(SidePanel));
  menu->addAction(actionList.value(DrawMode));
  menu->addAction(actionList.value(Crosshairs));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Tools"));
  menu->addAction(actionList.value(DataWindow1));
//  menu->addAction(actionList.value(Script));
  menubar->addMenu(menu);

  menubar->addSeparator();

  menu = new QMenu;
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
  Config config;
  if (config.getBool(Config::ShowQuitButton))
    toolbar->addAction(actionList.value(Exit));

  if (config.getBool(Config::ShowPrefButton))
    toolbar->addAction(actionList.value(Options));

  if (config.getBool(Config::ShowSidePanelButton))
    toolbar->addAction(actionList.value(SidePanel));

  if (config.getBool(Config::ShowGridButton))
    toolbar->addAction(actionList.value(Grid));

  if (config.getBool(Config::ShowScaleToScreenButton))
    toolbar->addAction(actionList.value(ScaleToScreen));

  if (config.getBool(Config::ShowCrosshairButton))
    toolbar->addAction(actionList.value(Crosshairs));

  if (config.getBool(Config::ShowDrawModeButton))
    toolbar->addAction(actionList.value(DrawMode));

  if (config.getBool(Config::ShowRefreshButton))
    toolbar->addAction(actionList.value(Refresh));

  if (config.getBool(Config::ShowNewIndicatorButton))
    toolbar->addAction(actionList.value(NewIndicator));

  if (config.getBool(Config::ShowDataWindowButton))
    toolbar->addAction(actionList.value(DataWindow1));

//  toolbar->addAction(actionList.value(Script));

  if (config.getBool(Config::ShowHelpButton))
    toolbar->addAction(actionList.value(Help));

  // construct the chart toolbar
  QToolBar *toolbar2 = addToolBar("chartToolBar");

  QString ts; // temporary string
  BarData bd;
  QStringList l;
  bd.getBarLengthList(l);

  // compression
  compressionCombo = new QComboBox;
  compressionCombo->addItems(l);
  compressionCombo->setCurrentIndex(config.getInt(Config::BarLength));
  compressionCombo->setToolTip(tr("Bar Length (Compression)"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(slotBarLengthChanged(int)));

  QAction *action = toolbar2->addWidget(compressionCombo);
  actionList.insert(Compression, action);

  // monthly compression button
  QToolButton *b = new QToolButton; // compression button monthly
  b->setToolTip(tr("Monthly Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnMClicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(CompressionM, action);
  action->setVisible(config.getBool(Config::ShowCmpsMtyButton));
  b->setText("M");

  // weekly compression button
  b = new QToolButton; // compression button weekly
  b->setToolTip(tr("Weekly Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnWClicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(CompressionW, action);
  action->setVisible(config.getBool(Config::ShowCmpsWkyButton));
  b->setText("W");

  // daily compression button
  b = new QToolButton; //   button daily
  b->setToolTip(tr("Daily Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtnDClicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(CompressionD, action);
  action->setVisible(config.getBool(Config::ShowCmpsDayButton));
  b->setText("D");

  // 60 minute compression button
  b = new QToolButton; // compression button 60min
  b->setToolTip(tr("60min Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn60Clicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(Compression60, action);
  action->setVisible(config.getBool(Config::ShowCmps60Button));
  b->setText("60");

  // 15 minute compression button
  b = new QToolButton; // compression button 15min
  b->setToolTip(tr("15min Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn15Clicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(Compression15, action);
  action->setVisible(config.getBool(Config::ShowCmps15Button));
  b->setText("15");

  // 5 minute compression button
  b = new QToolButton; // compression button 5min
  b->setToolTip(tr("5min Compression"));
  connect(b, SIGNAL(clicked()), this, SLOT(cmpsBtn5Clicked()));

  action = toolbar2->addWidget(b);
  actionList.insert(Compression5, action);
  action->setVisible(config.getBool(Config::ShowCmps5Button));
  b->setText("5");

  toolbar2->addSeparator();

  int ti = config.getInt(Config::Pixelspace);
  Setting set;
  set.setData(0, 0); // save index, 0 for now
  set.setData(1, ti); // save pixelspace
  zoomList.append(set);
  zoomPos = 0;

  // zoom in button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(slotZoomIn()));
  action = toolbar2->addWidget(b);
  actionList.insert(ZoomIn, action);
  b->setToolTip(tr("Zoom In"));
  b->setIcon(QIcon(zoomin_xpm));
  action->setEnabled(FALSE);
  action->setStatusTip(tr("Zoom In"));

  // zoom out button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(slotZoomOut()));
  action = toolbar2->addWidget(b);
  actionList.insert(ZoomOut, action);
  b->setToolTip(tr("Zoom Out"));
  b->setIcon(QIcon(zoomout_xpm));
  action->setEnabled(FALSE);
  action->setStatusTip(tr("Zoom Out"));


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


  toolbar2->addSeparator();

  //bars to load
  barCount = new QSpinBox;
  barCount->setRange(1, 9999);
  barCount->setValue(config.getInt(Config::BarsToLoad));
  barCount->setToolTip(tr("Total bars to load"));
  connect(barCount, SIGNAL(editingFinished()), this, SLOT(slotChartUpdated()));

  action = toolbar2->addWidget(barCount);
  actionList.insert(BarCount, action);

  toolbar2->addSeparator();

  // recent charts combo
  config.getData(Config::RecentChartsList, l);
  recentCharts = new QComboBox;
  recentCharts->setMaxCount(10);
  recentCharts->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  if (l.count())
    recentCharts->addItems(l);
  recentCharts->setCurrentIndex(0);
  recentCharts->setToolTip(tr("Recent Charts"));
  connect(recentCharts, SIGNAL(activated(int)), this, SLOT(slotOpenChart(int)));

  action = toolbar2->addWidget(recentCharts);
  actionList.insert(RecentCharts, action);
  action->setVisible(config.getBool(Config::ShowRecentCharts));
}

void QtstalkerApp::slotQuit()
{
  Config config;

  // do this to save any pending chart object edits
  emit signalClearIndicator();

  config.transaction();

  // save last indicators used
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    QString s = tw->tabText(tw->currentIndex());
    l.append(s);
  }
  config.setData(Config::LastIndicatorUsed, l);

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
  config.setData(Config::DrawMode, actionList.value(DrawMode)->isChecked());
  config.setData(Config::ScaleToScreen, actionList.value(ScaleToScreen)->isChecked());
  config.setData(Config::Grid, actionList.value(Grid)->isChecked());
  config.setData(Config::Crosshairs, actionList.value(Crosshairs)->isChecked());
  config.setData(Config::ShowSidePanel, actionList.value(SidePanel)->isChecked());
  config.setData(Config::RefreshStatus, actionList.value(Refresh)->isChecked());

  // save toolbar settings
  config.setData(Config::BarsToLoad, barCount->value());
  config.setData(Config::BarLength, compressionCombo->currentIndex());

  Setting set = zoomList[0];
  config.setData(Config::Pixelspace, set.getInt(1)); // save base zoom amount

  // save recent charts combo
  l.clear();
  for (loop = 0; loop < recentCharts->count(); loop++)
   l.append(recentCharts->itemText(loop));
  config.setData(Config::RecentChartsList, l);

  config.commit();

  // delete any BarData
  if (recordList)
    delete recordList;

//  delete script;
}

void QtstalkerApp::closeEvent(QCloseEvent *)
{
  delete assistant;
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  QString versionString = "Qtstalker\nVersion 0.37-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2001-2009 by Stefan Stratigakos\nqtstalker.sourceforge.net";
  versionString += "\nQtstalker is licensed with GNU General Public License (GPL) version 2.";
  versionString += "\nQt Assistant is licensed with GNU General Public License (GPL) version 3.";
  QMessageBox::about(this, tr("About Qtstalker"), versionString);
}

void QtstalkerApp::slotStartDocumentation()
{
/*
FIXME: Due to the Qt issue 262508 (see docs/docs.html) we need to show them
how to remove the stale cache file. This is complicated to report the location
on different OSs (but perhaps i do not understand).
This workaround should all go away when the Qt bug is fixed, but only if we
raise the minimum Qt version.
*/
  QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#ifdef Q_WS_MAC
  location.insert(location.count() - QCoreApplication::applicationName().count(),
    QCoreApplication::organizationName() + "/");
#endif
  qDebug("QtstalkerApp::slotStartDocumentation: Documentation cache: %s/", qPrintable(location));

  // start assistant
  assistant->showDocumentation("index.html");
}

void QtstalkerApp::slotShowDocumentation(QString doc)
{
  assistant->showDocumentation(doc);
}

void QtstalkerApp::slotOpenChart (QString selection)
{
  // load a chart slot
  slider->setEnabled(TRUE);
  qApp->processEvents();
  loadChart(selection);
}

// sent here if user selected chart from recent charts combobox
// move selected item to the top of the list so we can order list starting from most recent
void QtstalkerApp::slotOpenChart (int row)
{
  QString s = recentCharts->itemText(row);
  recentCharts->removeItem(row);
  recentCharts->insertItem(0, s);
  recentCharts->setCurrentIndex(0);
  slotOpenChart(s);
}

void QtstalkerApp::slotOptions ()
{
  Preferences *dialog = new Preferences(this);
  connect(dialog, SIGNAL(signalBackgroundColor(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));
  connect(dialog, SIGNAL(signalBorderColor(QColor)), this, SIGNAL(signalBorderColor(QColor)));
  connect(dialog, SIGNAL(signalGridColor(QColor)), this, SIGNAL(signalGridColor(QColor)));
  connect(dialog, SIGNAL(signalPlotFont(QFont)), this, SIGNAL(signalPlotFont(QFont)));
  connect(dialog, SIGNAL(signalAppFont(QFont)), this, SLOT(slotAppFont(QFont)));
  connect(dialog, SIGNAL(signalLoadChart()), this, SLOT(slotChartUpdated()));
  connect(dialog, SIGNAL(signalRefreshChanged(int)), this, SLOT(slotRefreshUpdated(int)));
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

  // create and populate the quote data
  if (recordList)
    delete recordList;
  recordList = new BarData;
  recordList->setSymbol(chartPath);
  recordList->setBarLength((BarData::BarLength) compressionCombo->currentIndex());
  recordList->setBarsRequested(barCount->value());

  DataBase db;
  db.getChart(recordList);

  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop;
  for (loop = 0; loop < indicatorList.count(); loop++)
  {
    Indicator i;
    i.setName(indicatorList[loop]);
    db.getIndicator(i);

    QList<PlotLine *> lines;
    IndicatorFactory fac;
    QString s;
    i.getIndicator(s);
    IndicatorBase *ib = fac.getFunction(s);
    if (! ib)
      continue;

    ib->setSettings(i);

    if (ib->getIndicator(i, recordList))
    {
      qDebug() << "Qtstalker::loadChart: getIndicator failed";
      delete ib;
      continue;
    }

    delete ib;

    i.getLines(lines);

    Plot *plot = plotList[indicatorList[loop]];
    if (! plot)
      continue;

    plot->setData(recordList);
    plot->calculate();
    plot->getIndicatorPlot()->setPlotList(lines);
    plot->loadChartObjects();
  }

  resetZoomSettings();
  setSliderStart();

  Setting set = zoomList[0];
  emit signalPixelspace(set.getInt(1));

  emit signalIndex(slider->value());
  slotDrawPlots();
  setWindowTitle(getWindowCaption());
  slotStatusMessage(QString());
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text

  QString caption = tr("Qtstalker");

  if (! recordList)
    return caption;

  QString s;
  recordList->getSymbol(s);
  caption.append(": " + s);

  QString s2;
  recordList->getName(s2);
  if (! s2.isEmpty())
    caption.append(" (" + s2 + ")");

  caption.append(" " + compressionCombo->currentText());

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
  dw->scrollToBottom();
}

void QtstalkerApp::slotBarLengthChanged(int barLength)
{
  Config config;
  int n = config.getInt(Config::BarLength);
  if (n == barLength)
    return;

  // the compression has changed
  config.setData(Config::BarLength, compressionCombo->currentIndex());

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
  // add a new indicator
  addIndicatorButton(n);

  if (! recordList)
    return;
}

void QtstalkerApp::slotDeleteIndicator (QString text)
{
  // delete indicator
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

  if (! recordList)
    return;
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  DataBase db;
  Indicator i;
  i.setName(d);
  db.getIndicator(i);

  if (! i.getEnable())
    return;

  if (i.getTabRow() > tabList.count())
    return;

  QTabWidget *it = tabList.at(i.getTabRow() - 1);

  Plot *plot = new Plot(baseWidget);
  plotList.insert(d, plot);
  plot->setDateFlag(i.getDate());
  plot->setLogScale(i.getLog());

  it->addTab(plot, d);

  // get the last used indicators
  Config config;
  QStringList lastIndicatorUsed;
  config.getData(Config::LastIndicatorUsed, lastIndicatorUsed);

  // Set the current indicator in this row to the last used one.
  if (i.getTabRow() < lastIndicatorUsed.count())
  {
    if (d == lastIndicatorUsed[i.getTabRow()])
      it->setCurrentWidget(plot);
  }

  QColor color;
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

  Setting set = zoomList[zoomPos];
  plot->setPixelspace(set.getInt(1));

  plot->setIndex(slider->value());
  plot->setInterval((BarData::BarLength) compressionCombo->currentIndex());
  plot->setCrosshairsStatus(config.getBool(Config::Crosshairs));
  plot->setDrawMode(actionList.value(DrawMode)->isChecked());

  IndicatorPlot *indy = plot->getIndicatorPlot();
  indy->setIndicator(d);

  connect(indy, SIGNAL(signalPixelspaceChanged(int, int)), this, SLOT(slotPlotZoom(int, int)));
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
  chartNav->updateList();

  if (! recordList)
    return;

  Config config;
  config.setData(Config::BarsToLoad, barCount->value());

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
  connect(chartNav, SIGNAL(addRecentChart(QString)), this, SLOT(slotAddRecentChart(QString)));
  connect(chartNav, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  navTab->addTab(chartNav, QIcon(plainitem), QString());
  navTab->setTabToolTip(0, tr("Charts"));
}

void QtstalkerApp::initGroupNav ()
{
  GroupPage *gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  connect(chartNav, SIGNAL(signalAddToGroup()), gp, SLOT(updateGroups()));
  connect(gp, SIGNAL(addRecentChart(QString)), this, SLOT(slotAddRecentChart(QString)));
  navTab->addTab(gp, QIcon(dirclosed), QString());
  navTab->setTabToolTip(1, tr("Groups"));
}

void QtstalkerApp::initIndicatorNav ()
{
  IndicatorPage *ip = new IndicatorPage(baseWidget);
  connect(ip, SIGNAL(signalDisableIndicator(QString)), this, SLOT(slotDisableIndicator(QString)));
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(slotEnableIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(QString)), this, SLOT(slotNewIndicator(QString)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  connect(this, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  navTab->addTab(ip, QIcon(indicator), QString());
  navTab->setTabToolTip(2, tr("Indicators"));
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
  slotDrawPlots();
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
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

void QtstalkerApp::slotAppFont (QFont d)
{
  qApp->setFont(d, 0);
  slotWakeup();
}

/**********************************************************************/
/************************ SCRIPT FUNCTIONS ***************************/
/**********************************************************************/

// runs a Qtstalker script selected by user
/*
void QtstalkerApp::slotScript ()
{
  QString file = QFileDialog::getOpenFileName(this,
					      QString(tr("Select Qtstalker script to run.")),
					      QDir::homePath(),
					      QString(),
					      0,
					      0);
  if (file.isEmpty())
    return;

  bool ok;
  QString command = "perl " + file;
  command = QInputDialog::getText(this,
				  QString(tr("Enter Qtstalker script command")),
				  QString(tr("Script command")),
				  QLineEdit::Normal,
				  command,
				  &ok,
				  0);

  if (! ok)
    return;

  script->calculate(command);
}
*/

// called each time a Qtstalker script has executed
/*
void QtstalkerApp::slotScriptDone ()
{
  QMessageBox::information(this,
			   QString(tr("Script Completed")),
			   QString(tr("Script Completed")),
			   QMessageBox::Ok,
			   QMessageBox::Ok);
}
*/

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart ()
{
  QTabWidget *tw = tabList.at(0);
  Plot *plot = plotList[tw->tabText(tw->currentIndex())];
  if (! plot)
    return;

  Setting set = zoomList[0];
  int page = plot->getWidth() / set.getInt(1);
  int max = recordList->count() - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, recordList->count() - 1);

  if (recordList->count() < page)
  {
    slider->setValue(0);
    set.setData(0, 0);
  }
  else
  {
    slider->setValue(max + 1);
    set.setData(0, max + 1);
  }
  slider->blockSignals(FALSE);

  zoomList[0] = set;
}

void QtstalkerApp::ps1ButtonClicked ()
{
  Config config;
  int ti = config.getInt(Config::PSButton1);

  Setting set = zoomList[0];
  set.setData(1, ti);
  zoomList[0] = set;

  setSliderStart();
  emit signalPixelspace(ti);
  emit signalIndex(slider->value());
  slotDrawPlots();

  zoomPos = 0;
  if (zoomList.count() > 1)
    actionList.value(ZoomIn)->setEnabled(TRUE);
  actionList.value(ZoomOut)->setEnabled(FALSE);
}

void QtstalkerApp::ps2ButtonClicked ()
{
  Config config;
  int ti = config.getInt(Config::PSButton2);

  Setting set = zoomList[0];
  set.setData(1, ti);
  zoomList[0] = set;

  setSliderStart();
  emit signalPixelspace(ti);
  emit signalIndex(slider->value());
  slotDrawPlots();

  zoomPos = 0;
  if (zoomList.count() > 1)
    actionList.value(ZoomIn)->setEnabled(TRUE);
  actionList.value(ZoomOut)->setEnabled(FALSE);
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

void QtstalkerApp::slotAddRecentChart (QString d)
{
  if (recentCharts->findText(d, Qt::MatchExactly) > -1)
    return;

  recentCharts->insertItem(0, d);
}

// ******************************************************************************
// **************************** Zoom Functions **********************************
// ******************************************************************************

void QtstalkerApp::slotZoomIn ()
{
  zoomPos++;
  if (zoomPos == zoomList.count() - 1)
  {
    actionList.value(ZoomIn)->setEnabled(FALSE);
    actionList.value(ZoomOut)->setEnabled(TRUE);
  }

  Setting set = zoomList[zoomPos];
  emit signalPixelspace(set.getInt(1));
  emit signalIndex(set.getInt(0));
  slider->setValue(set.getInt(0));
  slotDrawPlots();
}

void QtstalkerApp::slotZoomOut ()
{
  zoomPos--;
  if (zoomPos == 0)
  {
    actionList.value(ZoomIn)->setEnabled(TRUE);
    actionList.value(ZoomOut)->setEnabled(FALSE);
  }

  Setting set = zoomList[zoomPos];
  emit signalPixelspace(set.getInt(1));
  emit signalIndex(set.getInt(0));
  slider->setValue(set.getInt(0));
  slotDrawPlots();
}

void QtstalkerApp::slotPlotZoom (int i, int p)
{
  Setting set;
  set.setData(0, i);
  set.setData(1, p);
  zoomList.append(set);

  zoomPos++;

  actionList.value(ZoomOut)->setEnabled(TRUE);

  emit signalPixelspace(p);
  emit signalIndex(i);
  slider->setValue(i);
  slotDrawPlots();
}

void QtstalkerApp::resetZoomSettings ()
{
  // clear the zoomList, leave first item as starting base
  int loop;
  for (loop = 1; loop < zoomList.count(); loop++)
    zoomList.removeAt(loop);

  zoomPos = 0;

  actionList.value(ZoomIn)->setEnabled(FALSE);
  actionList.value(ZoomOut)->setEnabled(FALSE);
}

// ******************************************************************************
// ******************* Refresh Chart Functions **********************************
// ******************************************************************************

void QtstalkerApp::slotRefreshChart (bool status)
{
  if (status == TRUE)
  {
    Config config;
    int minutes = config.getInt(Config::Refresh);

    if (refreshTimer)
      delete refreshTimer;

    refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(slotReloadChart()));
    refreshTimer->start(60000 * minutes);
  }
  else
  {
    if (refreshTimer)
      delete refreshTimer;
    refreshTimer = 0;
  }
}

void QtstalkerApp::slotReloadChart ()
{
  if (chartPath.isEmpty())
    return;

  loadChart(chartPath);
}

void QtstalkerApp::slotRefreshUpdated (int minutes)
{
  if (actionList.value(Refresh)->isEnabled() && actionList.value(Refresh)->isChecked())
  {
    if (refreshTimer)
      delete refreshTimer;

    refreshTimer = new QTimer(this);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(slotReloadChart()));
    refreshTimer->start(60000 * minutes);
  }
}



