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
#include <QHBoxLayout>
#include <QTimer>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "Indicator.h"
#include "Setup.h"
#include "IndicatorPage.h"
#include "PluginFactory.h"
#include "DBPlugin.h"
#include "QuoteIndexDataBase.h"
#include "IndicatorPlugin.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorDataBase.h"
#include "COFactory.h"

#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/done.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/qtstalker.xpm"
#include "../pics/script.xpm"
#include "../pics/plugin.xpm"

QtstalkerApp::QtstalkerApp(QString session, QString asset)
{
  setWindowIcon(QIcon(qtstalker));
  QApplication::setOrganizationName("Qtstalker");

  Config config;
  Setup setup;
  setup.setup(config, session);

  createToolBars();

  createGUI(config);

  loadSettings(config);

  // check if we are going to display a chart from the command line
  if (asset.length())
  {
    _clAsset = asset;
    QTimer::singleShot(500, this, SLOT(commandLineAsset()));
  }

  _statusBar->showMessage(tr("Ready"), 2000);
}

void QtstalkerApp::createGUI (Config &config)
{
  _statusBar = statusBar();

  _baseWidget = new QWidget;
  setCentralWidget (_baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(0);
  _baseWidget->setLayout(hbox);

  // add the side toolBar first
  hbox->addWidget(_toolBar2);

  _navSplitter = new Splitter((int) Config::NavAreaSize);
  connect(this, SIGNAL(signalLoadSettings()), _navSplitter, SLOT(load()));
  connect(this, SIGNAL(signalSaveSettings()), _navSplitter, SLOT(save()));
  _navSplitter->setOrientation(Qt::Horizontal);
  hbox->addWidget(_navSplitter);

  // construct the chart areas
  QWidget *chartBase = new QWidget;
  _navSplitter->addWidget(chartBase);
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  chartBase->setLayout(vbox);

  _split = new Splitter((int) Config::PlotSizes);
  connect(this, SIGNAL(signalLoadSettings()), _split, SLOT(load()));
  connect(this, SIGNAL(signalSaveSettings()), _split, SLOT(save()));
  _split->setOrientation(Qt::Vertical);
  vbox->addWidget(_split);

  // build the tab rows
  int tabRows = config.getInt(Config::IndicatorTabRows);
  int loop;
  for (loop = 0; loop < tabRows; loop++)
  {
    TabWidget *it = new TabWidget(loop);
    _split->addWidget(it);
    connect(it, SIGNAL(currentChanged(QWidget *)), this, SLOT(drawPlots()));
    connect(this, SIGNAL(signalDeleteIndicatorTab(QString)), it, SLOT(deleteTab(QString)));
    connect(this, SIGNAL(signalSaveSettings()), it, SLOT(save()));
    connect(this, SIGNAL(signalLoadSettings()), it, SLOT(load()));
    _tabList.append(it);
  }

  _navBase = new QWidget;
  _navSplitter->addWidget(_navBase);
  vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  _navBase->setLayout(vbox);

  // setup the data panel splitter
  _dpSplitter = new Splitter((int) Config::DataPanelSize);
  connect(this, SIGNAL(signalLoadSettings()), _dpSplitter, SLOT(load()));
  connect(this, SIGNAL(signalSaveSettings()), _dpSplitter, SLOT(save()));
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

  setUnifiedTitleAndToolBarOnMac(TRUE);

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));
}

void QtstalkerApp::loadSettings (Config &config)
{
  // setup the initial indicators
  IndicatorDataBase idb;
  QStringList l;
  idb.getActiveIndicatorList(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Indicator i;
    i.setName(l[loop]);
    idb.getIndicator(i);

    if (i.tabRow() > _tabList.count())
      continue;

    addIndicatorButton(l[loop]);
  }

  // restore the size of the app
  QSize sz;
  config.getData(Config::MainWindowSize, sz);
  if (! sz.isValid())
  {
    sz = QSize(800, 600);
    config.setData(Config::MainWindowSize, sz);
  }
  resize(sz);

  // restore the position of the app
  QPoint p;
  config.getData((int) Config::MainWindowPos, p);
  if (p.isNull())
  {
    p = QPoint(0, 0); // 30,30 ?
    config.setData(Config::MainWindowPos, p);
  }
  move(p);

  // load gui class settings that need to now
  emit signalLoadSettings();
}

void QtstalkerApp::createToolBars ()
{
  // construct the left side toolbar
  _toolBar2 = new COToolBar;
  connect(_toolBar2, SIGNAL(signalCursorButtonClicked(int)), this, SIGNAL(signalCursorChanged(int)));
  connect(_toolBar2, SIGNAL(signalCOButtonClicked(QString)), this, SIGNAL(signalNewExternalChartObject(QString)));
  addToolBar(_toolBar2);
  _toolBar2->setOrientation(Qt::Vertical);

  //construct main toolbar
  QToolBar *toolbar = addToolBar("buttonToolBar");
  toolbar->setIconSize(QSize(18, 18));

  // add the toolbar actions
  QAction *a = new QAction(QIcon(done_xpm), tr("E&xit"), this);
  a->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  a->setStatusTip(tr("Quit Qtstalker"));
  a->setToolTip(tr("Quit Qtstalker"));
  connect(a, SIGNAL(activated()), qApp, SLOT(quit()));
  toolbar->addAction(a);

  // grid button
  _gridAction = new GridAction;
  connect(_gridAction, SIGNAL(signalChanged(bool)), this, SLOT(gridChanged(bool)));
  connect(_gridAction, SIGNAL(signalColorChanged(QColor)), this, SIGNAL(signalGridColor(QColor)));
  toolbar->addWidget(_gridAction);

  // refresh button
  _refreshAction = new RefreshAction;
  connect(_refreshAction, SIGNAL(signalRefresh()), this, SLOT(chartUpdated()));
  toolbar->addWidget(_refreshAction);

  a = new QAction(QIcon(datawindow), tr("&Data Window"), this);
  a->setShortcut(QKeySequence(Qt::ALT+Qt::Key_1));
  a->setStatusTip(tr("Show the data window"));
  a->setToolTip(tr("Show the data window"));
  connect(a, SIGNAL(activated()), this, SLOT(dataWindow()));
  toolbar->addAction(a);

  // docs button
  DocsAction *da = new DocsAction;
  toolbar->addWidget(da);
  
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

  // date range controls
  _dateRange = new DateRangeControl(toolbar);
  connect(_dateRange, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));

  // date range button
  _dateRangeButton = new DateRangeButton();
  connect(_dateRangeButton, SIGNAL(signalDateRangeChanged()), this, SLOT(chartUpdated()));
  toolbar->addWidget(_dateRangeButton);

  toolbar->addSeparator();

  // create recent charts combobox
  _recentCharts = new RecentCharts(toolbar);
  connect(_recentCharts, SIGNAL(signalChartSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(this, SIGNAL(signalSaveSettings()), _recentCharts, SLOT(save()));
  connect(this, SIGNAL(signalLoadSettings()), _recentCharts, SLOT(load()));
}

void QtstalkerApp::quit()
{
  // save everything that needs to be saved before app quits
  // classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  emit signalSaveSettings();
  
  Config config;
  config.transaction();

  // save app size and position
  QSize sz = size();
  config.setData((int) Config::MainWindowSize, sz);
  QPoint pt = pos();
  config.setData((int) Config::MainWindowPos, pt);

  config.commit();
}

void QtstalkerApp::loadChart (BarData symbol)
{
  // do all the stuff we need to do to load a chart
  if (symbol.getSymbol().length() == 0)
    return;

  _currentChart = symbol;

  // set plots to empty
  emit signalClearIndicator();

  // create and populate the quote data
  BarData bd = _currentChart;
  bd.setBarLength((Bar::BarLength) _barLengthButtons->getCurrentButton());
  bd.setDateRange(_dateRange->dateRange());
  bd.setDateRangeOverride(_dateRangeButton->isChecked());
  bd.setStartDate(_dateRangeButton->startDate());
  bd.setEndDate(_dateRangeButton->endDate());

  QuoteIndexDataBase idb;
  idb.getIndexData(&bd);

  DBPluginFactory fac;
  DBPlugin *qdb = fac.plugin(bd.getPlugin());
  if (! qdb)
  {
    qDebug() << "QtStalkerApp::loadChart: no DB plugin";
    return;
  }
  
  qdb->getBars(bd);

  setSliderStart(bd.count());
//  emit signalIndex(_plotSlider->getValue());

  setWindowTitle(getWindowCaption());
  statusMessage(QString());

  IndicatorDataBase db;
  QStringList indicatorList;
  db.getActiveIndicatorList(indicatorList);

  int loop;
  for (loop = 0; loop < indicatorList.count(); loop++)
  {
    Plot *plot = _plotList.value(indicatorList[loop]);
    if (! plot)
      continue;

    plot->loadIndicator(bd, _plotSlider->getValue());
  }
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
  loadChart(_currentChart);
}

void QtstalkerApp::deleteIndicator (QString text)
{
  // delete indicator
  emit signalDeleteIndicatorTab(text);

  Plot *plot = _plotList.value(text);
  if (plot)
    delete plot;
  _plotList.remove(text);
}

void QtstalkerApp::addIndicator (QString name)
{
  addIndicatorButton(name);
  loadChart(_currentChart);
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
  it->addTab(plot, d);

  plot->setIndicator(i);

  connect(plot, SIGNAL(signalBackgroundColorChanged(QColor)), this, SLOT(backgroundColorChanged(QColor)));
  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));

  connect(plot, SIGNAL(signalBorderColorChanged(QColor)), this, SLOT(borderColorChanged(QColor)));
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));

  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));

  connect(plot, SIGNAL(signalPlotFontChanged(QFont)), this, SLOT(plotFontChanged(QFont)));
  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));

  connect(plot, SIGNAL(signalPixelSpaceChanged(int, int)), _zoomButtons, SLOT(addZoom(int, int)));
  connect(plot, SIGNAL(signalInfoMessage(Setting *)), _infoPanel, SLOT(showInfo(Setting *)));
  connect(plot, SIGNAL(signalStatusMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(Bar::BarLength)), plot, SLOT(setInterval(Bar::BarLength)));
  connect(this, SIGNAL(signalClearIndicator()), plot, SLOT(clear()));
  connect(this, SIGNAL(signalSaveSettings()), plot, SLOT(clear()));
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

  loadChart(_currentChart);
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
  connect(_chartNav, SIGNAL(fileSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(_chartNav, SIGNAL(addRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
  connect(_chartNav, SIGNAL(signalReloadChart()), this, SLOT(chartUpdated()));
  connect(_chartNav, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  _navTab->addTab(_chartNav, QIcon(plainitem), QString());
  _navTab->setTabToolTip(0, tr("Charts"));
}

// create the group panel
void QtstalkerApp::initGroupNav ()
{
  _groupNav = new GroupPage(_baseWidget);
  connect(_groupNav, SIGNAL(fileSelected(BarData)), this, SLOT(loadChart(BarData)));
  connect(_chartNav, SIGNAL(signalAddToGroup()), _groupNav, SLOT(updateGroups()));
  connect(_groupNav, SIGNAL(addRecentChart(BarData)), _recentCharts, SLOT(addRecentChart(BarData)));
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
  connect(ip, SIGNAL(signalRefreshIndicator(QString)), this, SLOT(chartUpdated()));
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
  connect(this, SIGNAL(signalLoadSettings()), _scriptPage, SLOT(loadSavedScripts()));
  connect(this, SIGNAL(signalSaveSettings()), _scriptPage, SLOT(saveRunningScripts()));
  _navTab->addTab(_scriptPage, QIcon(script_xpm), QString());
  _navTab->setTabToolTip(3, tr("Scripts"));
}

// create the plugin panel
void QtstalkerApp::initPluginNav ()
{
  _pluginNav = new PluginPage(_baseWidget);
  connect(_pluginNav, SIGNAL(signalMessage(QString)), this, SLOT(statusMessage(QString)));
  connect(_pluginNav, SIGNAL(signalChartRefresh()), this, SLOT(chartUpdated()));
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

    Plot *p = (Plot *) it->currentWidget();
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

void QtstalkerApp::backgroundColorChanged (QColor d)
{
  emit signalBackgroundColor(d);
}

void QtstalkerApp::borderColorChanged (QColor d)
{
  emit signalBorderColor(d);
}

void QtstalkerApp::plotFontChanged (QFont d)
{
  emit signalPlotFont(d);
}

/**********************************************************************/
/************************ TOOLBAR FUNCTIONS ***************************/
/**********************************************************************/

void QtstalkerApp::setSliderStart (int count)
{
  if (_currentChart.getSymbol().isEmpty())
    return;

  // find largest plot width
  int width = 0;
  QHashIterator<QString, Plot *> it(_plotList);
  while (it.hasNext())
  {
     it.next();
     if (it.value()->width() > width)
       width = it.value()->width();
  }

  _plotSlider->setStart(count, width, _zoomButtons->getPixelSpace());
}

/**********************************************************************/
/************************ ZoomButtons Functions *********************/
/**********************************************************************/

void QtstalkerApp::psButtonClicked (int pixelSpace)
{
  emit signalPixelspace(pixelSpace);
  drawPlots();
}

void QtstalkerApp::zoomChanged (int pixelSpace, int index)
{
  emit signalPixelspace(pixelSpace);
  _plotSlider->setValue(index);
  drawPlots();
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
  loadChart(bd);
}

