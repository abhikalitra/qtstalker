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
#include <QVBoxLayout>
#include <math.h> // only for fabs()
#include <QtDebug>
#include <QToolButton>
#include <QApplication>
#include <QDesktopServices>
#include <QHBoxLayout>

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
#include "ScriptPage.h"
#include "QuoteDataBase.h"

#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/done.xpm"
#include "../pics/grid.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/configure.xpm"
#include "../pics/scaletoscreen.xpm"
#include "../pics/nav.xpm"
#include "../pics/qtstalker.xpm"
#include "../pics/help.xpm"
#include "../pics/crosshair.xpm"
#include "../pics/zoomin.xpm"
#include "../pics/zoomout.xpm"
#include "../pics/refresh.xpm"
#include "../pics/script.xpm"
//#include "../pics/buyarrow.xpm"
//#include "../pics/fib.xpm"
//#include "../pics/horizontal.xpm"
//#include "../pics/sellarrow.xpm"
//#include "../pics/text.xpm"
//#include "../pics/trend.xpm"
//#include "../pics/vertical.xpm"
#include "../pics/cursor_arrow.xpm"


QtstalkerApp::QtstalkerApp(QString session)
{
  recordList = 0;
  refreshTimer = 0;
  zoomPos = -1;
  setWindowIcon(QIcon(qtstalker));
  QApplication::setOrganizationName("Qtstalker");

  // setup the disk environment and init databases
  // order is important here
  Setup setup;
  setup.setupDirectories();

  Config config;
  config.init(session);

  DataBase db;
  db.init();

  setup.setup();

  QuoteDataBase qdb;
  QString dbFile;
  config.getData(Config::DbName, dbFile);
  qdb.init(dbFile);

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

  // add the side toolBar first
//  hbox->addWidget(toolBar2);

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
  initScriptNav();

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
  config.getBaseData((int) Config::AppFont, font);
  slotAppFont(font);

  // restore the splitter sizes
  config.getBaseData((int) Config::PlotSizes, split);
  config.getBaseData((int) Config::NavAreaSize, navSplitter);
  config.getBaseData((int) Config::DataPanelSize, dpSplitter);

  // restore the size of the app
  QSize sz;
  config.getBaseData((int) Config::MainWindowSize, sz);
  resize(sz);

  // restore the position of the app
  QPoint p;
  config.getBaseData((int) Config::MainWindowPos, p);
  move(p);

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));

  statusbar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

void QtstalkerApp::createActions ()
{
  // create the actions needed for menu and toolbar

  QAction *action  = new QAction(QIcon(done_xpm), tr("E&xit"), this);
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

  action = new QAction(QIcon(refresh_xpm), tr("&Refresh Chart"), this);
  action->setStatusTip(tr("Refresh chart every x minutes"));
  action->setToolTip(tr("Refresh chart every x minutes"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::RefreshStatus));
  connect(action, SIGNAL(toggled(bool)), this, SLOT(slotRefreshChart(bool)));
  actionList.insert(Refresh, action);
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
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Tools"));
  menu->addAction(actionList.value(DataWindow1));
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
/*
  // construct the side toolbar
  toolBar2 = addToolBar("sideToolBar");
  toolBar2->setOrientation(Qt::Vertical);


  toolBar2->addSeparator();

  // buy arrow button
  b = new QToolButton;
  b->setToolTip(tr("Buy Arrow Object"));
  b->setIcon(QIcon(buyarrow_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(buyArrowButtonPressed()));
  toolBar2->addWidget(b);

  // fibo arrow button
  b = new QToolButton;
  b->setToolTip(tr("Retracement Level Object"));
  b->setIcon(QIcon(fib_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(fiboButtonPressed()));
  toolBar2->addWidget(b);

  // horizontal button
  b = new QToolButton;
  b->setToolTip(tr("Horizontal Line Object"));
  b->setIcon(QIcon(horizontal_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(horizontalButtonPressed()));
  toolBar2->addWidget(b);

  // sell arrow button
  b = new QToolButton;
  b->setToolTip(tr("Sell Arrow Object"));
  b->setIcon(QIcon(sellarrow_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(sellArrowButtonPressed()));
  toolBar2->addWidget(b);

  // text button
  b = new QToolButton;
  b->setToolTip(tr("Text Object"));
  b->setIcon(QIcon(text_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(textButtonPressed()));
  toolBar2->addWidget(b);

  // trend line button
  b = new QToolButton;
  b->setToolTip(tr("Trend Line Object"));
  b->setIcon(QIcon(trend_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(trendButtonPressed()));
  toolBar2->addWidget(b);

  // vertical button
  b = new QToolButton;
  b->setToolTip(tr("Vertical Line Object"));
  b->setIcon(QIcon(vertical_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(verticalButtonPressed()));
  toolBar2->addWidget(b);
*/

  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");

  toolbar->addAction(actionList.value(Exit));
  toolbar->addAction(actionList.value(Options));
  toolbar->addAction(actionList.value(SidePanel));
  toolbar->addAction(actionList.value(Grid));
  toolbar->addAction(actionList.value(ScaleToScreen));
  toolbar->addAction(actionList.value(Refresh));
  toolbar->addAction(actionList.value(NewIndicator));
  toolbar->addAction(actionList.value(DataWindow1));
  toolbar->addAction(actionList.value(Help));

  toolbar->addSeparator();

  QButtonGroup *bg = new QButtonGroup(this);
  connect(bg, SIGNAL(buttonClicked(int)), this, SLOT(cursorButtonPressed(int)));

  // normal cursor button
  QToolButton *b = new QToolButton;
  b->setToolTip(tr("Select Cursor"));
  b->setIcon(QIcon(cursor_arrow_xpm));
  b->setCheckable(TRUE);
  b->setChecked(TRUE);
  toolbar->addWidget(b);
  bg->addButton(b, 0);

  // crosshair button
  b = new QToolButton;
  b->setToolTip(tr("Crosshairs Cursor"));
  b->setIcon(QIcon(crosshair));
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  bg->addButton(b, 1);


  toolbar->addSeparator();


  // button group for the bars group
  barButtonGroup = new QButtonGroup(this);
  connect(barButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotBarLengthChanged(int)));

  // monthly compression button
  b = new QToolButton;
  b->setToolTip(tr("Monthly Bars"));
  b->setText("M");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 8);

  // weekly compression button
  b = new QToolButton;
  b->setToolTip(tr("Weekly Bars"));
  b->setText("W");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 7);

  // daily compression button
  b = new QToolButton;
  b->setToolTip(tr("Daily Bars"));
  b->setText("D");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 6);

  // 60 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("60 Minute Bars"));
  b->setText("60");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 5);

  // 30 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("30 Minute Bars"));
  b->setText("30");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 4);

  // 15 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("15 Minute Bars"));
  b->setText("15");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 3);

  // 10 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("10 Minute Bars"));
  b->setText("10");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 2);

  // 5 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("5 Minute Bars"));
  b->setText("5");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 1);

  // 1 minute compression button
  b = new QToolButton;
  b->setToolTip(tr("1 Minute Bars"));
  b->setText("1");
  b->setCheckable(TRUE);
  toolbar->addWidget(b);
  barButtonGroup->addButton(b, 0);

  // set the button to last used position
  Config config;
  b = (QToolButton *) barButtonGroup->button(config.getInt((int) Config::BarLength));
  b->setChecked(TRUE);


  toolbar->addSeparator();

  int ti = config.getInt(Config::Pixelspace);
  Setting set;
  set.setData(0, 0); // save index, 0 for now
  set.setData(1, ti); // save pixelspace
  zoomList.append(set);
  zoomPos = 0;

  // zoom in button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(slotZoomIn()));
  QAction *action = toolbar->addWidget(b);
  actionList.insert(ZoomIn, action);
  b->setToolTip(tr("Zoom In"));
  b->setIcon(QIcon(zoomin_xpm));
  action->setEnabled(FALSE);
  action->setStatusTip(tr("Zoom In"));

  // zoom out button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(slotZoomOut()));
  action = toolbar->addWidget(b);
  actionList.insert(ZoomOut, action);
  b->setToolTip(tr("Zoom Out"));
  b->setIcon(QIcon(zoomout_xpm));
  action->setEnabled(FALSE);
  action->setStatusTip(tr("Zoom Out"));


  // PS1 button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(ps1ButtonClicked()));
  QString ts;
  action = toolbar->addWidget(b);
  actionList.insert(PS1, action);
  config.getData(Config::PSButton1, ts);
  b->setToolTip(tr("Set Bar Spacing to ") + ts);
  b->setText(ts);

  // PS2 button
  b = new QToolButton;
  connect(b, SIGNAL(clicked()), this, SLOT(ps2ButtonClicked()));
  action = toolbar->addWidget(b);
  actionList.insert(PS2, action);
  config.getData(Config::PSButton2, ts);
  b->setToolTip(tr("Set Bar Spacing to ") + ts);
  b->setText(ts);

  toolbar->addSeparator();

  //bars to load
  barCount = new QSpinBox;
  barCount->setRange(1, 9999);
  barCount->setValue(config.getInt(Config::BarsToLoad));
  barCount->setToolTip(tr("Total bars to load"));
  connect(barCount, SIGNAL(editingFinished()), this, SLOT(slotChartUpdated()));
  action = toolbar->addWidget(barCount);
  actionList.insert(BarCount, action);

  toolbar->addSeparator();

  // recent charts combo
  QStringList l;
  config.getBaseData((int) Config::RecentChartsList, l);
  recentCharts = new QComboBox;
  recentCharts->setMaxCount(10);
  recentCharts->setSizeAdjustPolicy(QComboBox::AdjustToContents);
  if (l.count())
    recentCharts->addItems(l);
  recentCharts->setCurrentIndex(0);
  recentCharts->setToolTip(tr("Recent Charts"));
  connect(recentCharts, SIGNAL(activated(int)), this, SLOT(slotOpenChart(int)));
  action = toolbar->addWidget(recentCharts);
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
  config.setBaseData((int) Config::LastIndicatorUsed, l);

  // save window sizes
  config.setBaseData((int) Config::PlotSizes, split);
  config.setBaseData((int) Config::DataPanelSize, dpSplitter);
  config.setBaseData((int) Config::NavAreaSize, navSplitter);

  // save app size and position
  QSize sz = size();
  config.setBaseData((int) Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setBaseData((int) Config::MainWindowPos, pt);

  // save menubar settings
  config.setBaseData((int) Config::ScaleToScreen, actionList.value(ScaleToScreen)->isChecked());
  config.setBaseData((int) Config::Grid, actionList.value(Grid)->isChecked());
  config.setBaseData((int) Config::ShowSidePanel, actionList.value(SidePanel)->isChecked());
  config.setBaseData((int) Config::RefreshStatus, actionList.value(Refresh)->isChecked());

  // save toolbar settings
  config.setBaseData((int) Config::BarsToLoad, barCount->value());
  config.setBaseData((int) Config::BarLength, barButtonGroup->checkedId());

  Setting set = zoomList[0];
  config.setBaseData((int) Config::Pixelspace, set.getInt(1)); // save base zoom amount

  // save recent charts combo
  l.clear();
  for (loop = 0; loop < recentCharts->count(); loop++)
   l.append(recentCharts->itemText(loop));
  config.setBaseData((int) Config::RecentChartsList, l);

  config.commit();

  // delete any BarData
  if (recordList)
    delete recordList;
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
  recordList->setBarLength((BarData::BarLength) barButtonGroup->checkedId());
  recordList->setBarsRequested(barCount->value());

  QuoteDataBase qdb;
  qdb.getChart(recordList);

  DataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop;
  for (loop = 0; loop < indicatorList.count(); loop++)
    loadIndicator(indicatorList[loop]);

  resetZoomSettings();
  setSliderStart();

  Setting set = zoomList[0];
  emit signalPixelspace(set.getInt(1));

  emit signalIndex(slider->value());
  slotDrawPlots();
  setWindowTitle(getWindowCaption());
  slotStatusMessage(QString());
}

void QtstalkerApp::loadIndicator (QString &d)
{
  Indicator i;
  DataBase db;
  i.setName(d);
  db.getIndicator(i);

  QList<PlotLine *> lines;
  IndicatorFactory fac;
  QString s;
  i.getIndicator(s);
  IndicatorBase *ib = fac.getFunction(s);
  if (! ib)
    return;

  ib->setSettings(i);

  if (ib->getIndicator(i, recordList))
  {
    qDebug() << "Qtstalker::loadIndicator: getIndicator failed";
    delete ib;
    return;
  }

  delete ib;

  i.getLines(lines);

  Plot *plot = plotList[d];
  if (! plot)
    return;

  plot->setData(recordList);
  plot->calculate();
  plot->getIndicatorPlot()->setPlotList(lines);
  plot->loadChartObjects();
}

void QtstalkerApp::refreshIndicator (QString d)
{
  loadIndicator(d);
  slotDrawPlots();
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

//  caption.append(" " + compressionCombo->currentText());

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
  emit signalInterval((BarData::BarLength) barLength);
  loadChart(chartPath);
}

void QtstalkerApp::slotNewIndicator (QString n)
{
  // add a new indicator
  addIndicatorButton(n);

  if (! recordList)
    return;

  loadIndicator(n);
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

  loadIndicator(name);
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

  Config config;
  QColor color;
  config.getBaseData((int) Config::BackgroundColor, color);

  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  config.getBaseData((int) Config::BorderColor, color);
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  config.getBaseData((int) Config::GridColor, color);
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QFont font;
  config.getBaseData((int) Config::PlotFont, font);
  plot->setPlotFont(font);

  plot->setGridFlag(actionList.value(Grid)->isChecked());
  plot->setScaleToScreen(actionList.value(ScaleToScreen)->isChecked());

  Setting set = zoomList[zoomPos];
  plot->setPixelspace(set.getInt(1));

  plot->setIndex(slider->value());
  plot->setInterval((BarData::BarLength) barButtonGroup->checkedId());
  plot->setCrosshairsStatus(config.getBool(Config::Crosshairs));

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

//  connect(this, SIGNAL(signalNewExternalChartObject(int)), indy, SLOT(newExternalChartObject(int)));
//  connect(indy, SIGNAL(signalNewExternalChartObjectDone()), this, SLOT(newExternalChartObjectDone()));
//  connect(this, SIGNAL(signalSetExternalChartObject(int)), indy, SLOT(setExternalChartObjectFlag(int)));
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->updateList();

  if (! recordList)
    return;

  Config config;
  config.setBaseData((int) Config::BarsToLoad, barCount->value());

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
  connect(ip, SIGNAL(signalRefreshIndicator(QString)), this, SLOT(refreshIndicator(QString)));
  navTab->addTab(ip, QIcon(indicator), QString());
  navTab->setTabToolTip(2, tr("Indicators"));
}

void QtstalkerApp::initScriptNav ()
{
  ScriptPage *sp = new ScriptPage(baseWidget);
  navTab->addTab(sp, QIcon(script_xpm), QString());
  navTab->setTabToolTip(3, tr("Scripts"));
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

void QtstalkerApp::slotAddRecentChart (QString d)
{
  if (recentCharts->findText(d, Qt::MatchExactly) > -1)
    return;

  recentCharts->insertItem(0, d);
}

void QtstalkerApp::cursorButtonPressed (int id)
{
  switch (id)
  {
    case 0:
      emit signalCrosshairsStatus(FALSE);
      break;
    case 1:
      emit signalCrosshairsStatus(TRUE);
      break;
    default:
      break;
  }
}

// ******************************************************************************
// **************************** Side Toolbar Functions **************************
// ******************************************************************************
/*
void QtstalkerApp::buyArrowButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COBuyArrow);
}

void QtstalkerApp::fiboButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COFiboLine);
}

void QtstalkerApp::horizontalButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COHorizontalLine);
}

void QtstalkerApp::sellArrowButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COSellArrow);
}

void QtstalkerApp::textButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COText);
}

void QtstalkerApp::trendButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COTrendLine);
}

void QtstalkerApp::verticalButtonPressed ()
{
  emit signalNewExternalChartObject((int) COSettings::COVerticalLine);
}

// this slot is connected to a plot that is triggered when mouse has clicked the plot
// during external chart object creation
// we then turn off all the plots waiting for a click from the mouse
void QtstalkerApp::newExternalChartObjectDone ()
{
  emit signalSetExternalChartObject(0);
}
*/

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



