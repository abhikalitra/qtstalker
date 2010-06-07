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
#include "PluginFactory.h"
#include "DBPluginFactory.h"
#include "DBPlugin.h"
#include "IndicatorPlugin.h"
#include "IndicatorPluginFactory.h"
#include "MiscPluginFactory.h"
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
#include "../pics/qtstalker.xpm"
#include "../pics/help.xpm"
#include "../pics/script.xpm"
#include "../pics/plugin.xpm"

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
  IndicatorPluginFactory ifac;
  ifac.setPluginList();
  
  DBPluginFactory dbfac;
  dbfac.setPluginList();

  MiscPluginFactory mfac;
  mfac.setPluginList();

  GroupDataBase gdb;
  gdb.init();

  ScriptDataBase sdb;
  sdb.init();

  IndicatorDataBase idb;
  idb.init();
  setup.setupDefaultIndicators();

  CODataBase codb;
  codb.init();

  setup.setupDefaultSymbol();
  
  _assistant = new Assistant;

  createActions();
  createMenuBar();
  createToolBars();

  _statusBar = statusBar();

  _baseWidget = new QWidget;
  setCentralWidget (_baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  _baseWidget->setLayout(hbox);

  // add the side toolBar first
  hbox->addWidget(_toolBar2);

  _navSplitter = new QSplitter;
  _navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(_navSplitter);

  // construct the chart areas
  QWidget *chartBase = new QWidget;
  _navSplitter->addWidget(chartBase);
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  chartBase->setLayout(vbox);

  _split = new QSplitter;
  _split->setOrientation(Qt::Vertical);
  vbox->addWidget(_split);

  // build the tab rows
  int tabRows = config.getInt(Config::IndicatorTabRows);
  int loop;
  for (loop = 0; loop < tabRows; loop++)
  {
    QTabWidget *it = new QTabWidget;
    _split->addWidget(it);
    it->setContentsMargins(0, 0, 0, 0);
    connect(it, SIGNAL(currentChanged(QWidget *)), this, SLOT(drawPlots()));
    _tabList.append(it);
  }

  _navBase = new QWidget;
  _navSplitter->addWidget(_navBase);
  vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  _navBase->setLayout(vbox);

  // setup the data panel splitter
  _dpSplitter = new QSplitter;
  _dpSplitter->setOrientation(Qt::Vertical);
  vbox->addWidget(_dpSplitter);

  // setup the side panels
  _navTab = new QTabWidget;
  _dpSplitter->addWidget(_navTab);
  
  // setup the plot slider box
  _plotSlider = new PlotSlider;
  _dpSplitter->addWidget(_plotSlider);
  connect (_plotSlider, SIGNAL(signalValueChanged(int)), this, SLOT(sliderChanged(int)));

  // setup the info panel area
  _infoPanel = new InfoPanel;
  _dpSplitter->addWidget(_infoPanel);

  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initScriptNav();
  initPluginNav();

  // setup the initial indicators
  QStringList l;
  idb.getActiveIndicatorList(l);
  for (loop = 0; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    idb.getIndicator(i);

    if (i.tabRow() > _tabList.count())
      continue;

    addIndicatorButton(l[loop]);
  }

  // set the app font
  QFont font;
  config.getData((int) Config::AppFont, font);
  appFont(font);

  // restore the splitter sizes
  config.getData((int) Config::PlotSizes, _split);
  config.getData((int) Config::NavAreaSize, _navSplitter);
  config.getData((int) Config::DataPanelSize, _dpSplitter);

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
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

  _statusBar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
  
  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
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
  _actionList.insert(Exit, action);

  action = new QAction(QIcon(indicator), tr("New &Indicator"), this);
  action->setStatusTip(tr("Add a new indicator to chart"));
  action->setToolTip(tr("Add a new indicator to chart"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalNewIndicator()));
  _actionList.insert(NewIndicator, action);

  action = new QAction(QIcon(configure_xpm), tr("Edit &Preferences"), this);
  action->setStatusTip(tr("Modify user preferences"));
  action->setToolTip(tr("Modify user preferences"));
  connect(action, SIGNAL(activated()), this, SLOT(options()));
  _actionList.insert(Options, action);

  Config config;
  action = new QAction(QIcon(gridicon), tr("Chart &Grid"), this);
  action->setStatusTip(tr("Toggle the chart grid"));
  action->setToolTip(tr("Toggle the chart grid"));
  action->setCheckable(TRUE);
  action->setChecked(config.getBool(Config::Grid));
  connect(action, SIGNAL(toggled(bool)), this, SLOT(gridChanged(bool)));
  _actionList.insert(Grid, action);

  action = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  action->setStatusTip(tr("Show the data window"));
  action->setToolTip(tr("Show the data window"));
  connect(action, SIGNAL(activated()), this, SLOT(dataWindow()));
  _actionList.insert(DataWindow1, action);

  action = new QAction(QIcon(qtstalker), tr("&About"), this);
  action->setStatusTip(tr("About Qtstalker."));
  action->setToolTip(tr("About Qtstalker."));
  connect(action, SIGNAL(activated()), this, SLOT(about()));
  _actionList.insert(About, action);

  action = new QAction(QIcon(help), tr("&Help"), this);
  action->setStatusTip(tr("Show documentation."));
  action->setToolTip(tr("Show documentation."));
  connect(action, SIGNAL(triggered()), this, SLOT(startDocumentation()));
  _actionList.insert(Help, action);
}

void QtstalkerApp::createMenuBar()
{
  // create the main menubar
  QMenuBar *menubar = menuBar();

  QMenu *menu = new QMenu;
  menu->setTitle(tr("&File"));
  menu->addAction(_actionList.value(Exit));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Edit"));
  menu->addAction(_actionList.value(NewIndicator));
  menu->addAction(_actionList.value(Options));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&View"));
  menu->addAction(_actionList.value(Grid));
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Tools"));
  menu->addAction(_actionList.value(DataWindow1));
  menubar->addMenu(menu);

  menubar->addSeparator();

  menu = new QMenu;
  menu->setTitle(tr("&Help"));
  menu->addAction(_actionList.value(About));
  menu->addAction(_actionList.value(Help));
  menubar->addMenu(menu);
}

void QtstalkerApp::createToolBars ()
{
  // construct the left side toolbar
  _toolBar2 = new COToolBar;
  connect(_toolBar2, SIGNAL(signalCursorButtonClicked(int)), this, SIGNAL(signalCursorChanged(int)));
  connect(_toolBar2, SIGNAL(signalCOButtonClicked(QString)), this, SIGNAL(signalNewExternalChartObject(QString)));
  addToolBar(_toolBar2);
  _toolBar2->setOrientation(Qt::Vertical);
  _toolBar2->setIconSize(QSize(18, 18));
  
  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setIconSize(QSize(18, 18));

  // add the toolbar actions
  toolbar->addAction(_actionList.value(Exit));
  toolbar->addAction(_actionList.value(Options));
  toolbar->addAction(_actionList.value(Grid));
  
  RefreshAction *ra = new RefreshAction(toolbar);
  connect(ra, SIGNAL(signalRefresh()), this, SLOT(refreshChart()));
  connect(this, SIGNAL(signalRefreshUpdated(int)), ra, SLOT(refreshUpdated(int)));
  
  toolbar->addAction(_actionList.value(NewIndicator));
  toolbar->addAction(_actionList.value(DataWindow1));
  toolbar->addAction(_actionList.value(Help));
  toolbar->addSeparator();

  // create the bar length button group
  _barLengthButtons = new BarLengthButtons(toolbar);
  connect(_barLengthButtons, SIGNAL(signalBarLengthChanged(int)), this, SLOT(barLengthChanged(int)));
  toolbar->addSeparator();

  // create the zoom button box on the main toolbar
  _zoomButtons = new ZoomButtons(toolbar);
  connect(_zoomButtons, SIGNAL(signalZoom(int, int)), this, SLOT(zoomChanged(int, int)));
  connect(_zoomButtons, SIGNAL(signalPixelSpace(int)), this, SLOT(psButtonClicked(int)));
  toolbar->addSeparator();
  
  //bars to load
  Config config;
  _barCount = new QSpinBox;
  _barCount->setRange(1, 99999);
  _barCount->setValue(config.getInt(Config::BarsToLoad));
  _barCount->setToolTip(tr("Total bars to load"));
  connect(_barCount, SIGNAL(editingFinished()), this, SLOT(chartUpdated()));
  toolbar->addWidget(_barCount);
  toolbar->addSeparator();

  // create recent charts combobox
  _recentCharts = new RecentCharts(toolbar);
  connect(_recentCharts, SIGNAL(signalChartSelected(BarData *)), this, SLOT(loadChart(BarData *)));
}

void QtstalkerApp::quit()
{
  Config config;

  // do this to save any pending chart object edits
  emit signalClearIndicator();

  config.transaction();

  // save last indicators used
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) _tabList.count(); loop++)
  {
    QTabWidget *tw = _tabList.at(loop);
    QString s = tw->tabText(tw->currentIndex());
    l.append(s);
  }
  config.setData((int) Config::LastIndicatorUsed, l);

  // save window sizes
  config.setData((int) Config::PlotSizes, _split);
  config.setData((int) Config::DataPanelSize, _dpSplitter);
  config.setData((int) Config::NavAreaSize, _navSplitter);

  // save app size and position
  QSize sz = size();
  config.setData((int) Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setData((int) Config::MainWindowPos, pt);

  // save toolbar settings
  config.setData((int) Config::Grid, _actionList.value(Grid)->isChecked());
  config.setData((int) Config::BarsToLoad, _barCount->value());

  // save recent charts combo
  _recentCharts->save();

  // save running scripts
  _scriptPage->saveRunningScripts();
  
  delete _assistant;
  
  config.commit();
}

void QtstalkerApp::about()
{
  // display the about dialog
  QString versionString = "Qtstalker version 0.37-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2001-2010 by Stefan Stratigakos\nqtstalker.sourceforge.net";
  versionString += "\nQtstalker is licensed with GNU General Public License (GPL) version 2.";
  versionString += "\nQt Assistant is licensed with GNU General Public License (GPL) version 3.";
  QMessageBox::about(this, tr("About Qtstalker"), versionString);
}

void QtstalkerApp::startDocumentation()
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
  qDebug("QtstalkerApp::startDocumentation: Documentation cache: %s/", qPrintable(location));

  // start _assistant
  _assistant->showDocumentation("index.html");
}

void QtstalkerApp::showDocumentation(QString doc)
{
  _assistant->showDocumentation(doc);
}

void QtstalkerApp::options ()
{
  Preferences *dialog = new Preferences(this);
  connect(dialog, SIGNAL(signalBackgroundColor(QColor)), this, SIGNAL(signalBackgroundColor(QColor)));
  connect(dialog, SIGNAL(signalBorderColor(QColor)), this, SIGNAL(signalBorderColor(QColor)));
  connect(dialog, SIGNAL(signalGridColor(QColor)), this, SIGNAL(signalGridColor(QColor)));
  connect(dialog, SIGNAL(signalPlotFont(QFont)), this, SIGNAL(signalPlotFont(QFont)));
  connect(dialog, SIGNAL(signalAppFont(QFont)), this, SLOT(appFont(QFont)));
  connect(dialog, SIGNAL(signalLoadChart()), this, SLOT(chartUpdated()));
  connect(dialog, SIGNAL(signalRefreshChanged(int)), this, SIGNAL(signalRefreshUpdated(int)));
  connect(dialog, SIGNAL(signalPS1Changed(int)), _zoomButtons, SLOT(ps1ValueChanged(int)));
  connect(dialog, SIGNAL(signalPS2Changed(int)), _zoomButtons, SLOT(ps2ValueChanged(int)));
  dialog->show();
}

void QtstalkerApp::loadChart (BarData *symbol)
{
  // do all the stuff we need to do to load a chart
  if (symbol->getSymbol().length() == 0)
    return;

  _currentChart.setSymbol(symbol->getSymbol());
  _currentChart.setExchange(symbol->getExchange());

  // set plots to empty
  emit signalClearIndicator();

  // save the new chart
  Config config;
  QStringList l;
  l.append(_currentChart.getExchange());
  l.append(_currentChart.getSymbol());
  config.setData(Config::CurrentChart, l);

  // create and populate the quote data
  BarData recordList;
  recordList.setSymbol(_currentChart.getSymbol());
  recordList.setExchange(_currentChart.getExchange());
  recordList.setBarLength((Bar::BarLength) _barLengthButtons->getCurrentButton());
  recordList.setBarsRequested(_barCount->value());

  DBPlugin qdb;
  qdb.getIndexData(&recordList);

  DBPluginFactory fac;
  DBPlugin *qdb2 = fac.plugin(recordList.getPlugin());
  if (! qdb2)
  {
    qDebug() << "QtStalkerApp::loadChart: no DB plugin";
    return;
  }
  
  qdb2->getBars(recordList);
  
  _currentChart.setName(recordList.getName());
  _currentChart.setBarsRequested(recordList.count());

  IndicatorDataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop;
  for (loop = 0; loop < indicatorList.count(); loop++)
    loadIndicator(&recordList, indicatorList[loop]);

  setSliderStart(_currentChart.getBarsRequested());
  emit signalIndex(_plotSlider->getValue());

  drawPlots();
  
  setWindowTitle(getWindowCaption());
  
  statusMessage(QString());
}

void QtstalkerApp::loadIndicator (BarData *recordList, QString &d)
{
  Indicator i;
  IndicatorDataBase db;
  i.setName(d);
  db.getIndicator(i);

  IndicatorPluginFactory fac;
  QString s;
  s = i.indicator();
  IndicatorPlugin *ip = fac.plugin(s);
  if (! ip)
    return;

  ip->setSettings(i);
  if (ip->getIndicator(i, recordList))
    return;

  Plot *plot = _plotList[d];
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
  caption.append(": " + _currentChart.getSymbol());
  if (! _currentChart.getName().isEmpty())
    caption.append(" (" + _currentChart.getName() + ")");

  Bar bar;
  QStringList l;
  bar.getBarLengthList(l);
  caption.append(" " + l[_barLengthButtons->getCurrentButton()]);

  return caption;
}

void QtstalkerApp::dataWindow ()
{
  // show the datawindow dialog
  DataWindow *dw = new DataWindow(this);
  dw->setWindowTitle(getWindowCaption());
  dw->setData(_plotList);
  dw->show();
  dw->scrollToBottom();
}

// the bar length button was changed, update the charts
void QtstalkerApp::barLengthChanged(int barLength)
{
  emit signalInterval((Bar::BarLength) barLength);
  loadChart(&_currentChart);
}

void QtstalkerApp::deleteIndicator (QString text)
{
  // delete indicator
  int loop;
  for (loop = 0; loop < _tabList.count(); loop++)
  {
    QTabWidget *it = _tabList.at(loop);
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

  Plot *plot = _plotList.value(text);
  if (plot)
    delete plot;
  _plotList.remove(text);
}

void QtstalkerApp::addIndicator (QString name)
{
  addIndicatorButton(name);
  loadChart(&_currentChart);
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  IndicatorDataBase db;
  Indicator i;
  i.setName(d);
  db.getIndicator(i);

  if (! i.enable())
    return;

  if (i.tabRow() > _tabList.count())
    return;

  QTabWidget *it = _tabList.at(i.tabRow() - 1);

  Plot *plot = new Plot(_baseWidget);
  _plotList.insert(d, plot);
  plot->setDateFlag(i.date());
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

  plot->setGridFlag(_actionList.value(Grid)->isChecked());

  plot->setPixelspace(_zoomButtons->getPixelSpace());

  plot->setIndex(_plotSlider->getValue());
  plot->setInterval((Bar::BarLength) _barLengthButtons->getCurrentButton());

  connect(plot, SIGNAL(signalPixelSpaceChanged(int, int)), _zoomButtons, SLOT(addZoom(int, int)));
  connect(plot, SIGNAL(signalInfoMessage(Setting *)), _infoPanel, SLOT(showInfo(Setting *)));
  connect(plot, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(Bar::BarLength)), plot, SLOT(setInterval(Bar::BarLength)));
  connect(this, SIGNAL(signalClearIndicator()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalGrid(bool)), plot, SLOT(gridChanged(bool)));
  connect(this, SIGNAL(signalNewExternalChartObject(QString)), plot, SLOT(newExternalChartObject(QString)));
  connect(plot, SIGNAL(signalNewExternalChartObjectDone()), this, SIGNAL(signalSetExternalChartObject()));
  connect(this, SIGNAL(signalSetExternalChartObject()), plot, SLOT(setExternalChartObjectFlag()));
  connect(this, SIGNAL(signalCursorChanged(int)), plot, SLOT(cursorChanged(int)));
  connect(plot, SIGNAL(signalIndexChanged(int)), _plotSlider, SLOT(setValue(int)));
  connect(plot, SIGNAL(signalDraw()), this, SLOT(drawPlots()));
}

void QtstalkerApp::chartUpdated ()
{
  _chartNav->updateList();

  if (_currentChart.getSymbol().isEmpty())
    return;

  Config config;
  config.setData((int) Config::BarsToLoad, _barCount->value());

  loadChart(&_currentChart);
}

void QtstalkerApp::statusMessage (QString d)
{
  _statusBar->showMessage(d);
  wakeup();
}

// create the chart panel
void QtstalkerApp::initChartNav ()
{
  _chartNav = new ChartPage(_baseWidget);
  connect(_chartNav, SIGNAL(fileSelected(BarData *)), this, SLOT(loadChart(BarData *)));
  connect(_chartNav, SIGNAL(addRecentChart(BarData *)), _recentCharts, SLOT(addRecentChart(BarData *)));
  connect(_chartNav, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_chartNav, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  _navTab->addTab(_chartNav, QIcon(plainitem), QString());
  _navTab->setTabToolTip(0, tr("Charts"));
}

// create the group panel
void QtstalkerApp::initGroupNav ()
{
  _groupNav = new GroupPage(_baseWidget);
  connect(_groupNav, SIGNAL(fileSelected(BarData *)), this, SLOT(loadChart(BarData *)));
  connect(_chartNav, SIGNAL(signalAddToGroup()), _groupNav, SLOT(updateGroups()));
  connect(_groupNav, SIGNAL(addRecentChart(BarData *)), _recentCharts, SLOT(addRecentChart(BarData *)));
  connect(_groupNav, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  _navTab->addTab(_groupNav, QIcon(dirclosed), QString());
  _navTab->setTabToolTip(1, tr("Groups"));
}

// create the indicator panel
void QtstalkerApp::initIndicatorNav ()
{
  IndicatorPage *ip = new IndicatorPage(_baseWidget);
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(addIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(QString)), this, SLOT(addIndicator(QString)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(deleteIndicator(QString)));
  connect(this, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalRefreshIndicator(QString)), this, SLOT(refreshChart()));
  connect(ip, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  _navTab->addTab(ip, QIcon(indicator), QString());
  _navTab->setTabToolTip(2, tr("Indicators"));
}

// create the script panel
void QtstalkerApp::initScriptNav ()
{
  _scriptPage = new ScriptPage(_baseWidget);
  connect(_scriptPage, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_scriptPage, SIGNAL(signalScriptDone()), _chartNav, SLOT(updateList()));
  connect(_scriptPage, SIGNAL(signalScriptDone()), _groupNav, SLOT(updateList()));
  _navTab->addTab(_scriptPage, QIcon(script_xpm), QString());
  _navTab->setTabToolTip(3, tr("Scripts"));
}

// create the plugin panel
void QtstalkerApp::initPluginNav ()
{
  _pluginNav = new PluginPage(_baseWidget);
  connect(_pluginNav, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  _navTab->addTab(_pluginNav, QIcon(plugin_xpm), QString());
  _navTab->setTabToolTip(4, tr("Plugins"));
}

// draw the charts, but only the visible ones to save time
void QtstalkerApp::drawPlots ()
{
  int loop;
  for(loop = 0; loop < (int) _tabList.count(); loop++)
  {
    QTabWidget *it = _tabList.at(loop);
    if (it->isHidden())
      continue;
    
    QString s = it->tabText(it->currentIndex());
    Plot *p = _plotList[s];
    if (p)
      p->draw();
  }
}

void QtstalkerApp::wakeup ()
{
  qApp->processEvents();
}

void QtstalkerApp::appFont (QFont d)
{
  qApp->setFont(d, 0);
  wakeup();
}

void QtstalkerApp::sliderChanged (int d)
{
  emit signalIndex(d);
  drawPlots();
}

void QtstalkerApp::gridChanged (bool d)
{
  emit signalGrid(d);
  drawPlots();
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  QTabWidget *tw = _tabList.at(0);
  Plot *plot = _plotList[tw->tabText(tw->currentIndex())];
  if (! plot)
    return;

  _plotSlider->setStart(count, plot->width(), _zoomButtons->getPixelSpace());
}

/**********************************************************************/
/************************ ZoomButtons Functions *********************/
/**********************************************************************/

void QtstalkerApp::psButtonClicked (int pixelSpace)
{
  emit signalPixelspace(pixelSpace);
//  emit signalIndex(_plotSlider->getValue());
  drawPlots();
}

void QtstalkerApp::zoomChanged (int pixelSpace, int index)
{
  emit signalPixelspace(pixelSpace);
  _plotSlider->setValue(index);
  drawPlots();
}

// ******************************************************************************
// ******************* Refresh Chart Functions **********************************
// ******************************************************************************

void QtstalkerApp::refreshChart ()
{
  if (_currentChart.getSymbol().isEmpty())
    return;

  loadChart(&_currentChart);
}

// ******************************************************************************
// ******************* Command Line Asset ***************************************
// ******************************************************************************
// if the -asset command was issued from the command line, then we will load
// a chart on startup

void QtstalkerApp::commandLineAsset ()
{
  QStringList l = _clAsset.split(":");
  if (l.count() != 2)
  {
    qDebug() << "QtstalkerApp::commandLineAsset: invalid command line asset string" << _clAsset;
    return;
  }
  
  BarData bd;
  bd.setExchange(l[0]);
  bd.setSymbol(l[1]);
  loadChart(&bd);
}

