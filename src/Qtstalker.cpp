/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

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
#include <QtDebug>
#include <QToolButton>
#include <QApplication>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QTimer>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "Preferences.h"
#include "Indicator.h"
#include "Setup.h"
#include "Config.h"
#include "IndicatorPage.h"
#include "GroupPage.h"
#include "PluginFactory.h"
#include "IndicatorPlugin.h"
#include "DBPlugin.h"
#include "GroupDataBase.h"
#include "IndicatorDataBase.h"
#include "CODataBase.h"
#include "ScriptDataBase.h"
#include "COFactory.h"
#include "RefreshAction.h"

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
#include "../pics/script.xpm"

QtstalkerApp::QtstalkerApp(QString session, QString asset)
{
  setWindowIcon(QIcon(qtstalker));
  QApplication::setOrganizationName("Qtstalker");

  // setup the disk environment and init databases
  // order is important here
  Setup setup;
  setup.setupDirectories();

  Config config;
  config.init(session);

  DBPlugin qdb;
  QString dbFile;
  config.getData(Config::DbName, dbFile);
  qdb.init(dbFile);

  setup.setupExchanges();
  setup.setupFutures();

  // get complete plugin inventory
  PluginFactory pfac;
  QStringList l;
  QString path;
  config.getData(Config::IndicatorPluginPath, path);
  pfac.getPluginList(path, l);
  config.setData(Config::IndicatorPluginList, l);
  
  GroupDataBase gdb;
  gdb.init();

  ScriptDataBase sdb;
  sdb.init();

  IndicatorDataBase idb;
  idb.init();
  setup.setupDefaultIndicators();

  CODataBase codb;
  codb.init();

  assistant = new Assistant;

  createActions();
  createMenuBar();
  createToolBars();

  statusbar = statusBar();

  baseWidget = new QWidget;
  setCentralWidget (baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  baseWidget->setLayout(hbox);

  // add the side toolBar first
  hbox->addWidget(toolBar2);

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
  
  // setup the plot slider box
  plotSlider = new PlotSlider;
  dpSplitter->addWidget(plotSlider);

  // setup the info panel area
  infoPanel = new InfoPanel;
  dpSplitter->addWidget(infoPanel);

  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initScriptNav();

  // setup the initial indicators
  idb.getActiveIndicatorList(l);
  for (loop = 0; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    idb.getIndicator(i);

    if (i.getTabRow() > tabList.count())
      continue;

    addIndicatorButton(l[loop]);
  }

  // set the app font
  QFont font;
  config.getData((int) Config::AppFont, font);
  slotAppFont(font);

  // restore the splitter sizes
  config.getData((int) Config::PlotSizes, split);
  config.getData((int) Config::NavAreaSize, navSplitter);
  config.getData((int) Config::DataPanelSize, dpSplitter);

  // restore the size of the app
  QSize sz;
  config.getData((int) Config::MainWindowSize, sz);
  resize(sz);

  // restore the position of the app
  QPoint p;
  config.getData((int) Config::MainWindowPos, p);
  move(p);
  
  // clear current chart from config
  QString s("");
  config.setData(Config::CurrentChart, s);

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));

  statusbar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
  
  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    clAsset = asset;
    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
  }
}

void QtstalkerApp::createActions ()
{
  // create the actions needed for menu and toolbar

  QAction *action  = new QAction(QIcon(done_xpm), tr("E&xit"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  action->setStatusTip(tr("Quit Qtstalker"));
  action->setToolTip(tr("Quit Qtstalker"));
  connect(action, SIGNAL(activated()), qApp, SLOT(quit()));
  actionList.insert(Exit, action);

  action = new QAction(QIcon(indicator), tr("New &Indicator"), this);
  action->setStatusTip(tr("Add a new indicator to chart"));
  action->setToolTip(tr("Add a new indicator to chart"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalNewIndicator()));
  actionList.insert(NewIndicator, action);

  action = new QAction(QIcon(configure), tr("Edit &Preferences"), this);
  action->setStatusTip(tr("Modify user preferences"));
  action->setToolTip(tr("Modify user preferences"));
  connect(action, SIGNAL(activated()), this, SLOT(slotOptions()));
  actionList.insert(Options, action);

  Config config;
  action = new QAction(QIcon(gridicon), tr("Chart &Grid"), this);
  action->setStatusTip(tr("Toggle the chart grid"));
  action->setToolTip(tr("Toggle the chart grid"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::Grid));
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalGrid(bool)));
  actionList.insert(Grid, action);

  action = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  action->setStatusTip(tr("Show the data window"));
  action->setToolTip(tr("Show the data window"));
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
  action->setStatusTip(tr("Scale chart to current screen data"));
  action->setToolTip(tr("Scale chart to current screen data"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::ScaleToScreen));
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actionList.insert(ScaleToScreen, action);

  action = new QAction(QIcon(nav), tr("Side Pa&nel"), this);
  action->setStatusTip(tr("Toggle the side panel area from view"));
  action->setToolTip(tr("Toggle the side panel area from view"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::ShowSidePanel));
  connect(action, SIGNAL(toggled(bool)), this, SLOT(slotHideNav(bool)));
  actionList.insert(SidePanel, action);
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
  // construct the left side toolbar
  toolBar2 = new COToolBar;
  connect(toolBar2, SIGNAL(signalCursorButtonClicked(int)), this, SIGNAL(signalCursorChanged(int)));
  connect(toolBar2, SIGNAL(signalCOButtonClicked(QString)), this, SIGNAL(signalNewExternalChartObject(QString)));
  addToolBar(toolBar2);
  toolBar2->setOrientation(Qt::Vertical);
  toolBar2->setIconSize(QSize(18, 18));
  
  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setIconSize(QSize(18, 18));

  // add the toolbar actions
  toolbar->addAction(actionList.value(Exit));
  toolbar->addAction(actionList.value(Options));
  toolbar->addAction(actionList.value(SidePanel));
  toolbar->addAction(actionList.value(Grid));
  toolbar->addAction(actionList.value(ScaleToScreen));
  
  RefreshAction *ra = new RefreshAction(toolbar);
  connect(ra, SIGNAL(signalRefresh()), this, SLOT(refreshChart()));
  connect(this, SIGNAL(signalRefreshUpdated(int)), ra, SLOT(refreshUpdated(int)));
  
  toolbar->addAction(actionList.value(NewIndicator));
  toolbar->addAction(actionList.value(DataWindow1));
  toolbar->addAction(actionList.value(Help));
  toolbar->addSeparator();

  // create the bar length button group
  barLengthButtons = new BarLengthButtons(toolbar);
  connect(barLengthButtons, SIGNAL(signalBarLengthChanged(int)), this, SLOT(barLengthChanged(int)));
  toolbar->addSeparator();

  // create the zoom button box on the main toolbar
  zoomButtons = new ZoomButtons(toolbar);
  connect(zoomButtons, SIGNAL(signalZoom(int, int)), this, SLOT(zoomChanged(int, int)));
  connect(zoomButtons, SIGNAL(signalPixelSpace(int)), this, SLOT(psButtonClicked(int)));
  toolbar->addSeparator();
  
  //bars to load
  Config config;
  barCount = new QSpinBox;
  barCount->setRange(1, 99999);
  barCount->setValue(config.getInt(Config::BarsToLoad));
  barCount->setToolTip(tr("Total bars to load"));
  connect(barCount, SIGNAL(editingFinished()), this, SLOT(slotChartUpdated()));
  toolbar->addWidget(barCount);
  toolbar->addSeparator();

  // create recent charts combobox
  recentCharts = new RecentCharts(toolbar);
  connect(recentCharts, SIGNAL(signalChartSelected(BarData *)), this, SLOT(loadChart(BarData *)));
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
  config.setData((int) Config::LastIndicatorUsed, l);

  // save window sizes
  config.setData((int) Config::PlotSizes, split);
  config.setData((int) Config::DataPanelSize, dpSplitter);
  config.setData((int) Config::NavAreaSize, navSplitter);

  // save app size and position
  QSize sz = size();
  config.setData((int) Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setData((int) Config::MainWindowPos, pt);

  // save toolbar settings
  config.setData((int) Config::ScaleToScreen, actionList.value(ScaleToScreen)->isChecked());
  config.setData((int) Config::Grid, actionList.value(Grid)->isChecked());
  config.setData((int) Config::ShowSidePanel, actionList.value(SidePanel)->isChecked());
  config.setData((int) Config::BarsToLoad, barCount->value());

  // save recent charts combo
  recentCharts->save();

  // save running scripts
  scriptPage->saveRunningScripts();
  
  delete assistant;
  
  config.commit();
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  QString versionString = "Qtstalker\nVersion 0.37-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2001-2010 by Stefan Stratigakos\nqtstalker.sourceforge.net";
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

void QtstalkerApp::slotOptions ()
{
  Preferences *dialog = new Preferences(this);
  connect(dialog, SIGNAL(signalBackgroundColor(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));
  connect(dialog, SIGNAL(signalBorderColor(QColor)), this, SIGNAL(signalBorderColor(QColor)));
  connect(dialog, SIGNAL(signalGridColor(QColor)), this, SIGNAL(signalGridColor(QColor)));
  connect(dialog, SIGNAL(signalPlotFont(QFont)), this, SIGNAL(signalPlotFont(QFont)));
  connect(dialog, SIGNAL(signalAppFont(QFont)), this, SLOT(slotAppFont(QFont)));
  connect(dialog, SIGNAL(signalLoadChart()), this, SLOT(slotChartUpdated()));
  connect(dialog, SIGNAL(signalRefreshChanged(int)), this, SIGNAL(signalRefreshUpdated(int)));
  dialog->show();
}

void QtstalkerApp::loadChart (BarData *symbol)
{
  // do all the stuff we need to do to load a chart
  if (symbol->getSymbol().length() == 0)
    return;

  currentChart.setSymbol(symbol->getSymbol());
  currentChart.setExchange(symbol->getExchange());

  // set plots to empty
  emit signalClearIndicator();

  // save the new chart
  Config config;
  QStringList l;
  l.append(currentChart.getExchange());
  l.append(currentChart.getSymbol());
  config.setData(Config::CurrentChart, l);

  // create and populate the quote data
  BarData recordList;
  recordList.setSymbol(currentChart.getSymbol());
  recordList.setExchange(currentChart.getExchange());
  recordList.setBarLength((Bar::BarLength) barLengthButtons->getCurrentButton());
  recordList.setBarsRequested(barCount->value());

  DBPlugin qdb;
  qdb.getIndexData(&recordList);

  PluginFactory fac;
  QString path;
  config.getData(Config::DBPluginPath, path);
  DBPlugin *qdb2 = fac.getDB(path, recordList.getPlugin());
  if (! qdb2)
  {
    qDebug() << "QtStalkerApp::loadChart: no DB plugin";
    return;
  }
  
  qdb2->getBars(recordList);
  
  currentChart.setName(recordList.getName());
  currentChart.setBarsRequested(recordList.count());

  IndicatorDataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop;
  for (loop = 0; loop < indicatorList.count(); loop++)
    loadIndicator(&recordList, indicatorList[loop]);

  setSliderStart(currentChart.getBarsRequested());
  emit signalIndex(plotSlider->getValue()); 

  slotDrawPlots();
  
  setWindowTitle(getWindowCaption());
  
  slotStatusMessage(QString());
}

void QtstalkerApp::loadIndicator (BarData *recordList, QString &d)
{
  Indicator i;
  IndicatorDataBase db;
  i.setName(d);
  db.getIndicator(i);

  Config config;
  QString path;
  config.getData(Config::IndicatorPluginPath, path);
  
  PluginFactory fac;
  QString s;
  i.getIndicator(s);
  IndicatorPlugin *ip = fac.getIndicator(path, s);
  if (! ip)
    return;

  ip->setSettings(i);
  if (ip->getIndicator(i, recordList))
    return;

  Plot *plot = plotList[d];
  if (! plot)
    return;

  plot->setData(recordList);
  plot->setIndicator(i);
  plot->loadChartObjects();
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text

  QString caption = tr("Qtstalker");
  caption.append(": " + currentChart.getSymbol());
  if (! currentChart.getName().isEmpty())
    caption.append(" (" + currentChart.getName() + ")");

  Bar bar;
  QStringList l;
  bar.getBarLengthList(l);
  caption.append(" " + l[barLengthButtons->getCurrentButton()]);

  return caption;
}

void QtstalkerApp::slotDataWindow ()
{
  // show the datawindow dialog
  DataWindow *dw = new DataWindow(this);
  dw->setWindowTitle(getWindowCaption());
  dw->setData(plotList);
  dw->show();
  dw->scrollToBottom();
}

// the bar length button was changed, update the charts
void QtstalkerApp::barLengthChanged(int barLength)
{
  emit signalInterval((Bar::BarLength) barLength);
  loadChart(&currentChart);
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

void QtstalkerApp::addIndicator (QString name)
{
  addIndicatorButton(name);
  loadChart(&currentChart);
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  IndicatorDataBase db;
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
  config.getData((int) Config::BackgroundColor, color);

  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  config.getData((int) Config::BorderColor, color);
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  config.getData((int) Config::GridColor, color);
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QFont font;
  config.getData((int) Config::PlotFont, font);
  plot->setPlotFont(font);

  plot->setGridFlag(actionList.value(Grid)->isChecked());
  plot->setScaleToScreen(actionList.value(ScaleToScreen)->isChecked());

  plot->setPixelspace(zoomButtons->getPixelSpace());

  plot->setIndex(plotSlider->getValue());
  plot->setInterval((Bar::BarLength) barLengthButtons->getCurrentButton());

  connect(plot, SIGNAL(signalPixelspaceChanged(int, int)), zoomButtons, SLOT(addZoom(int, int)));
  connect(plot, SIGNAL(signalInfoMessage(Setting *)), infoPanel, SLOT(showInfo(Setting *)));
  connect(plot, SIGNAL(signalStatusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(Bar::BarLength)), plot, SLOT(setInterval(Bar::BarLength)));
  connect(this, SIGNAL(signalClearIndicator()), plot, SLOT(clear()));
  connect (plotSlider, SIGNAL(signalValueChanged(int)), plot, SLOT(sliderChanged(int)));
  connect(this, SIGNAL(signalGrid(bool)), plot, SLOT(gridChanged(bool)));
  connect(this, SIGNAL(signalScale(bool)), plot, SLOT(scaleToScreenChanged(bool)));
  connect(this, SIGNAL(signalNewExternalChartObject(QString)), plot, SLOT(newExternalChartObject(QString)));
  connect(plot, SIGNAL(signalNewExternalChartObjectDone()), this, SIGNAL(signalSetExternalChartObject()));
  connect(this, SIGNAL(signalSetExternalChartObject()), plot, SLOT(setExternalChartObjectFlag()));
  connect(this, SIGNAL(signalCursorChanged(int)), plot, SLOT(cursorChanged(int)));
  connect(plot, SIGNAL(signalIndexChanged(int)), plotSlider, SLOT(setValue(int)));
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->updateList();

  if (currentChart.getSymbol().isEmpty())
    return;

  Config config;
  config.setData((int) Config::BarsToLoad, barCount->value());

  loadChart(&currentChart);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusbar->showMessage(d);
  wakeup();
}

// create the chart panel
void QtstalkerApp::initChartNav ()
{
  chartNav = new ChartPage(baseWidget);
  connect(chartNav, SIGNAL(fileSelected(BarData *)), this, SLOT(loadChart(BarData *)));
  connect(chartNav, SIGNAL(addRecentChart(BarData *)), recentCharts, SLOT(addRecentChart(BarData *)));
  connect(chartNav, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  connect(chartNav, SIGNAL(signalMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addTab(chartNav, QIcon(plainitem), QString());
  navTab->setTabToolTip(0, tr("Charts"));
}

// create the group panel
void QtstalkerApp::initGroupNav ()
{
  GroupPage *gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(BarData *)), this, SLOT(loadChart(BarData *)));
  connect(chartNav, SIGNAL(signalAddToGroup()), gp, SLOT(updateGroups()));
  connect(gp, SIGNAL(addRecentChart(BarData *)), recentCharts, SLOT(addRecentChart(BarData *)));
  connect(gp, SIGNAL(signalMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addTab(gp, QIcon(dirclosed), QString());
  navTab->setTabToolTip(1, tr("Groups"));
}

// create the indicator panel
void QtstalkerApp::initIndicatorNav ()
{
  IndicatorPage *ip = new IndicatorPage(baseWidget);
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(addIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(QString)), this, SLOT(addIndicator(QString)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  connect(this, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalRefreshIndicator(QString)), this, SLOT(refreshChart()));
  connect(ip, SIGNAL(signalMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addTab(ip, QIcon(indicator), QString());
  navTab->setTabToolTip(2, tr("Indicators"));
}

// create the script panel
void QtstalkerApp::initScriptNav ()
{
  scriptPage = new ScriptPage(baseWidget);
  connect(scriptPage, SIGNAL(signalMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addTab(scriptPage, QIcon(script_xpm), QString());
  navTab->setTabToolTip(3, tr("Scripts"));
}

// enable / disable the side panels
void QtstalkerApp::slotHideNav (bool d)
{
  if (d)
    navBase->show();
  else
    navBase->hide();
}

// draw the charts, but only the visible ones to save time
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

void QtstalkerApp::wakeup ()
{
  qApp->processEvents();
}

void QtstalkerApp::slotAppFont (QFont d)
{
  qApp->setFont(d, 0);
  wakeup();
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  QTabWidget *tw = tabList.at(0);
  Plot *plot = plotList[tw->tabText(tw->currentIndex())];
  if (! plot)
    return;

  plotSlider->setStart(count, plot->width(), zoomButtons->getPixelSpace());
}

/**********************************************************************/
/************************ ZoomButtons Functions *********************/
/**********************************************************************/

void QtstalkerApp::psButtonClicked (int pixelSpace)
{
  emit signalPixelspace(pixelSpace);
  emit signalIndex(plotSlider->getValue());
  slotDrawPlots();
}

void QtstalkerApp::zoomChanged (int pixelSpace, int index)
{
  emit signalPixelspace(pixelSpace);
  plotSlider->setValue(index);
  slotDrawPlots();
}

// ******************************************************************************
// ******************* Refresh Chart Functions **********************************
// ******************************************************************************

void QtstalkerApp::refreshChart ()
{
  if (currentChart.getSymbol().isEmpty())
    return;

  loadChart(&currentChart);
}

// ******************************************************************************
// ******************* Command Line Asset ***************************************
// ******************************************************************************
// if the -asset command was issued from the command line, then we will load
// a chart on startup

void QtstalkerApp::commandLineAsset ()
{
  QStringList l = clAsset.split(":");
  if (l.count() != 2)
  {
    qDebug() << "QtstalkerApp::commandLineAsset: invalid command line asset string" << clAsset;
    return;
  }
  
  BarData bd;
  bd.setExchange(l[0]);
  bd.setSymbol(l[1]);
  loadChart(&bd);
}

