/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2004 by Stefan Stratigakos
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

#include <qlayout.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qdir.h>
#include <qfiledialog.h>
#include <qinputdialog.h>
#include <qapplication.h>
#include <qfont.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qcursor.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qstringlist.h>
#include <qtooltip.h>
#include <qframe.h>

#include "Qtstalker.h"
#include "Quote.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "IndicatorPage.h"
#include "PlotLine.h"
#include "BuyArrow.h"
#include "SellArrow.h"
#include "TrendLine.h"
#include "HorizontalLine.h"
#include "VerticalLine.h"
#include "FiboLine.h"
#include "Text.h"
#include "PrefDialog.h"

#include "grid.xpm"
#include "datawindow.xpm"
#include "indicator.xpm"
#include "quotes.xpm"
#include "done.xpm"
#include "configure.xpm"
#include "scaletoscreen.xpm"
#include "qtstalker.xpm"
#include "nav.xpm"
#include "dirclosed.xpm"
#include "plainitem.xpm"
#include "portfolio.xpm"
#include "test.xpm"
#include "loggrid.xpm"
#include "hidechart.xpm"
#include "date.xpm"
#include "co.xpm"
//#include "scanner.xpm"

QtstalkerApp::QtstalkerApp()
{
  config = 0;
  recordList = 0;
  status = None;
  plotList.setAutoDelete(TRUE);
  setIcon(qtstalker);

  QString s(QDir::homeDirPath());
  s.append("/Qtstalker/config");
  config = new Config(s);

  initActions();

  initMenuBar();

  baseWidget = new QWidget(this);
  setCentralWidget (baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout(baseWidget);

  navSplitter = new QSplitter(baseWidget);
  navSplitter->setOrientation(Horizontal);
  hbox->addWidget(navSplitter);
  
  navBase = new QWidget(navSplitter);
  QVBoxLayout *vbox = new QVBoxLayout(navBase);

  navTab = new QTabWidget(navBase);
  vbox->addWidget(navTab, 1, 0);

  infoLabel = new QMultiLineEdit(navBase);
  infoLabel->setReadOnly(TRUE);
  vbox->addWidget(infoLabel, 1, 0);

  // construct the chart areas

  QWidget *chartBase = new QWidget(navSplitter);

  vbox = new QVBoxLayout(chartBase);

  split = new QSplitter(chartBase);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  mainPlot = new Plot (split);
  mainPlot->setDateFlag(TRUE);
  mainPlot->setMainFlag(TRUE);
  mainPlot->setConfig(config);

  QColor color;
  color.setNamedColor(config->getData(Config::BackgroundColor));
  QObject::connect(this, SIGNAL(signalBackgroundColor(QColor)), mainPlot, SLOT(setBackgroundColor(QColor)));
  emit signalBackgroundColor(color);

  color.setNamedColor(config->getData(Config::BorderColor));
  QObject::connect(this, SIGNAL(signalBorderColor(QColor)), mainPlot, SLOT(setBorderColor(QColor)));
  emit signalBorderColor(color);

  color.setNamedColor(config->getData(Config::GridColor));
  QObject::connect(this, SIGNAL(signalGridColor(QColor)), mainPlot, SLOT(setGridColor(QColor)));
  emit signalGridColor(color);

  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), mainPlot, SLOT(setPlotFont(QFont)));
  emit signalPlotFont(font);

  QObject::connect(mainPlot, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  QObject::connect(mainPlot, SIGNAL(signalEditIndicator(QString, Plot *)), this, SLOT(slotEditIndicator(QString, Plot *)));
  QObject::connect(mainPlot, SIGNAL(signalDeleteIndicator(QString, Plot *)), this, SLOT(slotDeleteIndicator(QString, Plot *)));

  QObject::connect(mainPlot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(mainPlot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  QObject::connect(mainPlot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  QObject::connect(mainPlot, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotPlotKeyPressed(QKeyEvent *)));
  QObject::connect(mainPlot, SIGNAL(signalMinPixelspace(int)), this, SLOT(slotMinPixelspaceChanged(int)));
  QObject::connect(mainPlot, SIGNAL(signalCrosshairsStatus(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  QObject::connect(this, SIGNAL(signalCrosshairsStatus(bool)), mainPlot, SLOT(setCrosshairsStatus(bool)));

  QObject::connect(this, SIGNAL(signalGrid(bool)), mainPlot, SLOT(setGridFlag(bool)));
  QObject::connect(this, SIGNAL(signalScaleToScreen(bool)), mainPlot, SLOT(setScaleToScreen(bool)));
  QObject::connect(this, SIGNAL(signalPixelspace(int)), mainPlot, SLOT(setPixelspace(int)));
  QObject::connect(this, SIGNAL(signalIndex(int)), mainPlot, SLOT(setIndex(int)));
  QObject::connect(this, SIGNAL(signalInterval(ChartDb::BarCompression)), mainPlot, SLOT(setInterval(ChartDb::BarCompression)));
  QObject::connect(this, SIGNAL(signalChartPath(QString)), mainPlot, SLOT(setChartPath(QString)));
  QObject::connect(this, SIGNAL(signalDrawMode(bool)), mainPlot, SLOT(setDrawMode(bool)));

  tabs = new QTabWidget(split);
  if (config->getData(Config::IndicatorTabs).toInt() == 0)
    tabs->setTabPosition(QTabWidget::Bottom);
  QObject::connect(tabs, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged(QWidget *)));

  initToolBar();

  statusBar()->message(tr("Ready"), 2000);

  // set up the number of bars to load
  barCount->setValue(config->getData(Config::Bars).toInt());

  // setup the compression list
  ChartDb *db = new ChartDb();
  compressionCombo->insertStringList(db->getBarCompressionList(), -1);
  compressionCombo->setCurrentItem(config->getData(Config::Compression).toInt());
  compressionChanged();
  delete db;

  // set the grid status
  s = config->getData(Config::Grid);
  if (s.toInt())
    actionGrid->setOn(TRUE);
  else
    actionGrid->setOn(FALSE);
  emit signalGrid(s.toInt());

  // set the scale to screen status
  s = config->getData(Config::ScaleToScreen);
  if (s.toInt())
    actionScaleToScreen->setOn(TRUE);
  else
    actionScaleToScreen->setOn(FALSE);
  emit signalScaleToScreen(s.toInt());

  // set the indicator splitter size
  QValueList<int> sizeList;
  s = config->getData(Config::MainPlotSize);
  sizeList.append(s.toInt());
  s = config->getData(Config::IndicatorPlotSize);
  sizeList.append(s.toInt());
  split->setSizes(sizeList);

  // set the logscale status
  s = config->getData(Config::LogScale);
  if (s.toInt())
    actionLogScale->setOn(TRUE);
  else
    actionLogScale->setOn(FALSE);
  mainPlot->setLogScale(s.toInt());

  // set the nav splitter size
  sizeList.clear();
  s = config->getData(Config::NavAreaSize);
  sizeList.append(s.toInt());
  QString s2 = config->getData(Config::Width);
  sizeList.append(s2.toInt() - s.toInt());
  navSplitter->setSizes(sizeList);
  
  // place navigator on left/right side
  if (config->getData(Config::NavigatorPosition).toInt() == 0)
    navSplitter->moveToLast(navBase);

  l = config->getIndicators();
  QStringList sl = QStringList::split(",", config->getData(Config::StackedIndicator), FALSE);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (sl.findIndex(l[loop]) != -1)
      addIndicatorButton(l[loop], FALSE);
    else
      addIndicatorButton(l[loop], TRUE);
  }

  l = QStringList::split(" ", config->getData(Config::AppFont), FALSE);
  QFont font2(l[0], l[1].toInt(), l[2].toInt());
  qApp->setFont(font2, TRUE, 0);

  // set the nav status
  slotHideNav(TRUE);

  // set the last used chart type
  slotChartTypeChanged(0);

  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initPortfolioNav();
  initTestNav();
//  initScannerNav();

  resize(config->getData(Config::Width).toInt(), config->getData(Config::Height).toInt());
  
  // set crosshairs status for all the plots
  emit signalCrosshairsStatus(config->getData(Config::Crosshairs).toInt());  
  
  // set the drawmode status for all the plots
  s = config->getData(Config::DrawMode);
  if (s.toInt())
    actionDrawMode->setOn(TRUE);
  else
    actionDrawMode->setOn(FALSE);
  emit signalDrawMode(s.toInt());  
  
  // catch any kill signals and try to save config
  QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initActions()
{
  QPixmap icon(finished);
  actionQuit = new QAction(tr("Exit"), icon, tr("Exit"), 0, this);
  actionQuit->setStatusTip(tr("Quit Qtstalker."));
  connect(actionQuit, SIGNAL(activated()), qApp, SLOT(quit()));

  icon = indicator;
  actionNewIndicator = new QAction(tr("New Indicator..."), icon, tr("New Indicator..."), 0, this);
  actionNewIndicator->setStatusTip(tr("Add a new indicator to chart."));
  connect(actionNewIndicator, SIGNAL(activated()), this, SLOT(slotNewIndicator()));
  actionNewIndicator->setEnabled(FALSE);

  icon = configure;
  actionOptions = new QAction(tr("Edit Preferences..."), icon, tr("Edit Preferences..."), 0, this);
  actionOptions->setStatusTip(tr("Modify user preferences."));
  connect(actionOptions, SIGNAL(activated()), this, SLOT(slotOptions()));

  icon = gridicon;
  actionGrid = new QAction(tr("Chart Grid"), icon, tr("Chart Grid"), 0, this, 0, true);
  actionGrid->setStatusTip(tr("Toggle the chart grid."));
  connect(actionGrid, SIGNAL(toggled(bool)), this, SLOT(slotGrid(bool)));

  icon = quotes;
  actionQuotes = new QAction(tr("Quotes..."), icon, tr("Quotes..."), 0, this);
  actionQuotes->setStatusTip(tr("Download quotes from internet."));
  connect(actionQuotes, SIGNAL(activated()), this, SLOT(slotQuotes()));

  icon = datawindow;
  actionDatawindow = new QAction(tr("Data Window..."), icon, tr("Data Window..."), 0, this);
  actionDatawindow->setStatusTip(tr("Show the data window."));
  connect(actionDatawindow, SIGNAL(activated()), this, SLOT(slotDataWindow()));
  actionDatawindow->setEnabled(FALSE);

  icon = qtstalker;
  actionAbout = new QAction(tr("About"), icon, tr("&About..."), 0, this);
  actionAbout->setStatusTip(tr("About Qtstalker."));
  connect(actionAbout, SIGNAL(activated()), this, SLOT(slotAbout()));

  icon = scaletoscreen;
  actionScaleToScreen = new QAction(tr("Scale To Screen"), icon, tr("Scale To Screen"), 0, this, 0, true);
  actionScaleToScreen->setStatusTip(tr("Scale chart to current screen data."));
  connect(actionScaleToScreen, SIGNAL(toggled(bool)), this, SLOT(slotScaleToScreen(bool)));

  icon = nav;
  actionNav = new QAction(tr("Chart Navigator"), icon, tr("Chart Navigator"), 0, this, 0, true);
  actionNav->setStatusTip(tr("Toggle the chart navigator area."));
  connect(actionNav, SIGNAL(toggled(bool)), this, SLOT(slotHideNav(bool)));

  icon = loggridicon;
  actionLogScale = new QAction(tr("Log Scaling"), icon, tr("Log Scaling"), 0, this, 0, true);
  actionLogScale->setStatusTip(tr("Toggle log scaling."));
  connect(actionLogScale, SIGNAL(toggled(bool)), this, SLOT(slotLogScale(bool)));

  icon = hidechart;
  actionHideMainPlot = new QAction(tr("Hide Main Plot"), icon, tr("Hide Main Plot"), 0, this, 0, true);
  actionHideMainPlot->setStatusTip(tr("Hide the main plot."));
  connect(actionHideMainPlot, SIGNAL(toggled(bool)), this, SLOT(slotHideMainPlot(bool)));

  icon = date;
  actionPlotDate = new QAction(tr("Toggle Indicator Date"), icon, tr("Toggle Indicator Date"), 0, this, 0, true);
  actionPlotDate->setStatusTip(tr("Toggle indicator date."));
  connect(actionPlotDate, SIGNAL(toggled(bool)), this, SLOT(slotPlotDate(bool)));

  icon = co;
  actionDrawMode = new QAction(tr("Toggle Draw Mode"), icon, tr("Toggle Draw Mode"), 0, this, 0, true);
  actionDrawMode->setStatusTip(tr("Toggle drawing mode."));
  connect(actionDrawMode, SIGNAL(toggled(bool)), this, SLOT(slotDrawMode(bool)));
}

void QtstalkerApp::initMenuBar()
{
  fileMenu = new QPopupMenu();
  actionQuit->addTo(fileMenu);

  editMenu = new QPopupMenu();
  actionNewIndicator->addTo(editMenu);
  actionOptions->addTo(editMenu);

  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
  actionGrid->addTo(viewMenu);
  actionScaleToScreen->addTo(viewMenu);
  actionLogScale->addTo(viewMenu);
  actionNav->addTo(viewMenu);
  actionHideMainPlot->addTo(viewMenu);
  actionPlotDate->addTo(viewMenu);

  toolMenu = new QPopupMenu();
  actionDatawindow->addTo(toolMenu);
  actionQuotes->addTo(toolMenu);

  helpMenu = new QPopupMenu();
  actionAbout->addTo(helpMenu);

  menuBar()->insertItem(tr("&File"), fileMenu);
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertItem(tr("&Tools"), toolMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);
}

void QtstalkerApp::initToolBar()
{
  // construct the chart toolbar
  toolbar = new QToolBar(this, "toolbar");

  actionNav->addTo(toolbar);
  actionGrid->addTo(toolbar);
  actionScaleToScreen->addTo(toolbar);
  actionLogScale->addTo(toolbar);
  actionHideMainPlot->addTo(toolbar);
  actionPlotDate->addTo(toolbar);
  actionDrawMode->addTo(toolbar);
  actionNewIndicator->addTo(toolbar);
  actionDatawindow->addTo(toolbar);
  actionQuotes->addTo(toolbar);

  toolbar2 = new QToolBar(this, "toolbar2");

  compressionCombo = new QComboBox(toolbar2);
  compressionCombo->show();
  QToolTip::add(compressionCombo, tr("Chart Compression"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(slotCompressionChanged(int)));

  chartTypeCombo = new QComboBox(toolbar2);
  chartTypeCombo->show();
  chartTypeCombo->insertStringList(config->getChartPlugins(), -1);
  QToolTip::add(chartTypeCombo, tr("Chart Type"));
  chartTypeCombo->setCurrentText(config->getData(Config::ChartStyle));
  connect(chartTypeCombo, SIGNAL(activated(int)), this, SLOT(slotChartTypeChanged(int)));

  pixelspace = new QSpinBox(toolbar2);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));

  barCount = new QSpinBox(1, 99999999, 1, toolbar2);
  QToolTip::add(barCount, tr("Total bars to load"));

  toolbar2->addSeparator();

  slider = new QSlider(toolbar2);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));

  toolbar2->setStretchableWidget(slider);
}

void QtstalkerApp::slotQuit()
{
  // save any chart data
  mainPlot->slotSaveChartObjects();
  
  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->slotSaveChartObjects();

  // save app settings
  QValueList<int> list = split->sizes();
  config->setData(Config::MainPlotSize, QString::number(list[0]));
  config->setData(Config::IndicatorPlotSize, QString::number(list[1]));
  config->setData(Config::Height, QString::number(this->height()));
  config->setData(Config::Width, QString::number(this->width()));
  config->setData(Config::X, QString::number(this->x()));
  config->setData(Config::Y, QString::number(this->y()));
  config->setData(Config::Bars, QString::number(barCount->value()));
  config->setData(Config::Crosshairs, QString::number(mainPlot->getCrosshairsStatus()));
  config->setData(Config::DrawMode, QString::number(actionDrawMode->isOn()));
  config->closePlugins();
  delete config;

  // delete any BarData
  if (recordList)
    delete recordList;
}

void QtstalkerApp::slotAbout()
{
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					tr("Qtstalker\nVersion 0.26 \n(C) 2001-2004 by Stefan Stratigakos"),
					QMessageBox::NoIcon,
					QMessageBox::Ok,
					QMessageBox::NoButton,
					QMessageBox::NoButton,
					this,
					"AboutDialog",
					FALSE);
  dialog->setIconPixmap(qtstalker);
  dialog->show();
}

void QtstalkerApp::slotOpenChart (QString selection)
{
  slider->setEnabled(TRUE);
  actionDatawindow->setEnabled(TRUE);
  actionNewIndicator->setEnabled(TRUE);
  status = Chart;
  qApp->processEvents();
  loadChart(selection);
}

void QtstalkerApp::slotQuotes ()
{
  QuoteDialog *dialog = new QuoteDialog(config);
  QObject::connect(dialog, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
  QObject::connect(dialog, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  dialog->show();
}

void QtstalkerApp::slotOptions ()
{
  PrefDialog *dialog = new PrefDialog();
  dialog->setCaption(tr("Edit Prefs"));

  dialog->createPage(tr("Colors"));
  dialog->addColorItem(tr("Chart Background"), 1, QColor(config->getData(Config::BackgroundColor)));
  dialog->addColorItem(tr("Chart Border"), 1, QColor(config->getData(Config::BorderColor)));
  dialog->addColorItem(tr("Chart Grid"), 1, QColor(config->getData(Config::GridColor)));

  dialog->createPage(tr("Fonts"));
  dialog->addFontItem(tr("Plot Font"), 2, QFont(config->getData(Config::PlotFont)));
  dialog->addFontItem(tr("App Font"), 2, QFont(config->getData(Config::AppFont)));
    
  dialog->createPage(tr("Misc"));
  dialog->addCheckItem(tr("Navigator Left"), 3, config->getData(Config::NavigatorPosition).toInt());
  dialog->addCheckItem(tr("Indicator Tabs Top"), 3, config->getData(Config::IndicatorTabs).toInt());
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QColor color = dialog->getColor(tr("Chart Background"));
    config->setData(Config::BackgroundColor, color.name());
    emit signalBackgroundColor(color);

    color = dialog->getColor(tr("Chart Border"));
    config->setData(Config::BorderColor, color.name());
    emit signalBorderColor(color);

    color = dialog->getColor(tr("Chart Grid"));
    config->setData(Config::GridColor, color.name());
    emit signalGridColor(color);

    // save plot font option
    QFont font = dialog->getFont(tr("Plot Font"));
    QString s = font.family();
    s.append(" ");
    s.append(QString::number(font.pointSize()));
    s.append(" ");
    s.append(QString::number(font.weight()));
    config->setData(Config::PlotFont, s);
    emit signalPlotFont(font);

    // save app font option
    font = dialog->getFont(tr("App Font"));
    s = font.family();
    s.append(" ");
    s.append(QString::number(font.pointSize()));
    s.append(" ");
    s.append(QString::number(font.weight()));
    config->setData(Config::AppFont, s);
    qApp->setFont(font, TRUE, 0);

    // save navigator left option
    bool flag = dialog->getCheck(tr("Navigator Left"));
    if (flag)
    {
      config->setData(Config::NavigatorPosition, "1");
      navSplitter->moveToFirst(navBase);
    }
    else
    {
      config->setData(Config::NavigatorPosition, "0");
      navSplitter->moveToLast(navBase);
    }
    navSplitter->refresh();

    // save indicator tabs position option
    flag = dialog->getCheck(tr("Indicator Tabs Top"));
    if (flag)
    {
      config->setData(Config::IndicatorTabs, "1");
      tabs->setTabPosition(QTabWidget::Top);
    }
    else
    {
      config->setData(Config::IndicatorTabs, "0");
      tabs->setTabPosition(QTabWidget::Bottom);
    }

    loadChart(chartPath);

    statusBar()->message (tr("Preferences saved."));
  }

  delete dialog;
}

void QtstalkerApp::slotGrid (bool state)
{
  config->setData(Config::Grid, QString::number(state));
  
  emit signalGrid(state);

  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged (0);
}

void QtstalkerApp::slotScaleToScreen (bool state)
{
  config->setData(Config::ScaleToScreen, QString::number(state));

  emit signalScaleToScreen(state);

  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
}

void QtstalkerApp::slotLogScale (bool state)
{
  config->setData(Config::LogScale, QString::number(state));
  mainPlot->setLogScale(state);
  mainPlot->draw();
}

void QtstalkerApp::loadChart (QString d)
{
  if (d.length() == 0)
    return;

  QDir dir(d);
  if (! dir.exists(d, TRUE))
    return;

  // check if we need to save the slider position because chart is reloaded
  bool reload = FALSE;
  if (! chartPath.compare(d))
    reload = TRUE;

  chartPath = d;

  // get a list of disabled indicators before you kill them
  QStringList l = mainPlot->getIndicators();
  QStringList di;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = mainPlot->getIndicator(l[loop]);
    if (i->getEnable() == FALSE)
      di.append(l[loop]);
  }

  mainPlot->clear();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();
    
  // make sure we change db's after Plot saves previous db edits
  emit signalChartPath(chartPath);
  
  ChartDb *db = new ChartDb;
  db->setBarCompression((ChartDb::BarCompression) compressionCombo->currentItem());
  db->setBarRange(barCount->value());
  if (db->openChart(chartPath))
  {
    qDebug("QtstalkerApp::loadChart: error opening %s", chartPath.latin1());
    delete db;
    return;
  }
  
  if (recordList)
    delete recordList;
  recordList = db->getHistory();
  chartName = db->getDetail(ChartDb::Title);
  chartType = db->getDetail(ChartDb::Type);
  chartSymbol = db->getDetail(ChartDb::Symbol);
  
  bool otherFlag = FALSE;
// determine if chart is other type
//FIXME
/*  
  if (chartType.compare(tr("Stock"))
      && chartType.compare(tr("Futures"))
      && chartType.compare(tr("Index"))
      && chartType.compare(tr("Spread")))
  {
    chartTypeCombo->setEnabled(FALSE);
    mainPlot->setHideMainPlot(TRUE);
    actionHideMainPlot->setEnabled(FALSE);
    otherFlag = TRUE;
  }
  else
  {
    chartTypeCombo->setEnabled(TRUE);
    actionHideMainPlot->setEnabled(TRUE);
    if (! actionHideMainPlot->isOn())
      mainPlot->setHideMainPlot(FALSE);
  }
*/
  
  mainPlot->setData(recordList);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);
    
  mainPlot->setChartInput ();

  l = config->getIndicators();

  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = config->getIndicator(l[loop]);
    if (! set->count())
    {
      delete set;
      continue;
    }
    
    Indicator *i = new Indicator;
    i->setName(l[loop]);
    i->setFile(config->getData(Config::IndicatorPath) + "/" + l[loop]);
    i->setType(set->getData("plugin"));
    delete set;
    loadIndicator(i);

    if (otherFlag)
      i->clearLines();

    if (i->getMainPlot())
    {
      // restore the enable status of this main chart indicator
      if (di.findIndex(l[loop]) != -1)
	i->setEnable(FALSE);
    }
  }

// FIXME: Other type plots are disbaled for now
/*  
  if (recordList->count() && otherFlag)
  {
    QStringList keys = recordList->getFormat();

    Indicator *i = new Indicator;

    int loop;
    for (loop = 0; loop < (int) keys.count(); loop++)
    {
      PlotLine *pl = new PlotLine;
      int loop2;
      for (loop2 = 0; loop2 < (int) recordList->count(); loop2++)
	pl->append(recordList->getOther(loop2, loop));

      QString s = keys[loop];
      s.append(tr(" Color"));
      pl->setColor(set->getData(s));

      s = keys[loop];
      s.append(tr(" Line Type"));
      pl->setType(set->getData(s));
      
      pl->setLabel(keys[loop]);

      i->addLine(pl);
    }
    
    mainPlot->addIndicator(QString("Main Plot"), i);
  }
*/

  QList<Setting> *col = db->getChartObjects();
  for (loop = 0; loop < (int) col->count(); loop++)
  {
    Setting *co = col->at(loop);
    
    QString s = co->getData("Plot");
    if (! s.compare(tr("Main Plot")))
      mainPlot->addChartObject(co);
    else
    {
      Plot *plot = plotList[s];
      if (plot)
        plot->addChartObject(co);
    }
  }
  delete col;
  delete db;

  // update the pixelspace
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->blockSignals(FALSE);
  
  setSliderStart();
  
  mainPlot->draw();

  for(it.toFirst(); it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);

  setCaption(getWindowCaption());

  emit signalIndicatorPageRefresh();
}

void QtstalkerApp::loadIndicator (Indicator *i)
{
  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, i->getType());
  if (plug)
  {
    plug->setIndicatorInput(recordList);
    plug->loadIndicatorSettings(i->getFile());
    plug->calculate();

    int loop;
    for (loop = 0; loop < plug->getIndicatorLines(); loop++)
    {
      PlotLine *tpl = plug->getIndicatorLine(loop);
      PlotLine *pl = new PlotLine;
      pl->copy(tpl);
      i->addLine(pl);
    }

    // set up the paint bar
/*    
    QString s = config->getData(Config::PaintBarIndicator);
    if (! s.compare(i->getName()))
    {
      plug->getAlerts();
      mainPlot->setPaintBars(plug->getColorBars(config->getData(Config::UpColor),
      						config->getData(Config::DownColor),
						config->getData(Config::NeutralColor)));
    }
*/

    i->setMainPlot(plug->getPlotFlag());
    
    if (i->getMainPlot())
      mainPlot->addIndicator(i->getName(), i);
    else
    {
      Plot *plot = plotList[i->getName()];
      plot->addIndicator(i->getName(), i);
      plot->setData(recordList);
    }
  }
  
  config->closePlugin(i->getType());
}

QString QtstalkerApp::getWindowCaption ()
{
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
  DataWindow *dw = new DataWindow(0, recordList->count());
  dw->setCaption(getWindowCaption());

  dw->setHeader(0, tr("Date"));
  dw->setHeader(1, tr("Time"));
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    BarDate dt = recordList->getDate(loop);
    dw->setData(loop, 0, dt.getDateString(TRUE));
    dw->setData(loop, 1, dt.getTimeString(TRUE));
  }

  int col = 2;
  if (! mainPlot->getHideMainPlot())
  {
    dw->setHeader(2, tr("Open"));
    dw->setHeader(3, tr("High"));
    dw->setHeader(4, tr("Low"));
    dw->setHeader(5, tr("Close"));

    for (loop = 0; loop < (int) recordList->count(); loop++)
    {
      dw->setData(loop, 2, mainPlot->strip(recordList->getOpen(loop)));
      dw->setData(loop, 3, mainPlot->strip(recordList->getHigh(loop)));
      dw->setData(loop, 4, mainPlot->strip(recordList->getLow(loop)));
      dw->setData(loop, 5, mainPlot->strip(recordList->getClose(loop)));
    }

    col = 6;
  }

  QStringList l = mainPlot->getIndicators();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = mainPlot->getIndicator(l[loop]);
    int loop2;
    for (loop2 = 0; loop2 < i->getLines(); loop2++, col++)
    {
      PlotLine *line = i->getLine(loop2);
      dw->setHeader(col, line->getLabel());

      int loop3;
      int offset = recordList->count() - line->getSize();
      for (loop3 = 0; loop3 < line->getSize(); loop3++)
        dw->setData(loop3 + offset, col, mainPlot->strip(line->getData(loop3)));
    }
  }

  QDictIterator<Plot> it(plotList);
  for (; it.current(); ++it)
  {
    Plot *plot = it.current();
    l = plot->getIndicators();
    for (loop = 0; loop < (int) l.count(); loop++)
    {
      Indicator *i = plot->getIndicator(l[loop]);
      int loop2;
      for (loop2 = 0; loop2 < i->getLines(); loop2++, col++)
      {
        PlotLine *line = i->getLine(loop2);
        dw->setHeader(col, line->getLabel());

        int loop3;
        int offset = recordList->count() - line->getSize();
        for (loop3 = 0; loop3 < line->getSize(); loop3++)
          dw->setData(loop3 + offset, col, mainPlot->strip(line->getData(loop3)));
      }
    }
  }

  dw->show();
}

void QtstalkerApp::slotCompressionChanged (int)
{
  compressionChanged();
  loadChart(chartPath);
}

void QtstalkerApp::compressionChanged ()
{
  config->setData(Config::Compression, QString::number(compressionCombo->currentItem()));
  emit signalInterval((ChartDb::BarCompression) compressionCombo->currentItem());
}

void QtstalkerApp::slotChartTypeChanged (int)
{
  if (mainPlot->setChartType(chartTypeCombo->currentText()))
    return;

  config->setData(Config::ChartStyle, chartTypeCombo->currentText());

  emit signalPixelspace(mainPlot->getPixelspace());

  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->setValue(mainPlot->getMinPixelspace());
  pixelspace->blockSignals(FALSE);
  
  setSliderStart();

  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
}

void QtstalkerApp::slotNewIndicator ()
{
  PrefDialog *idialog = new PrefDialog();
  idialog->setCaption(tr("New Indicator"));
  idialog->createPage (tr("Details"));
  idialog->addComboItem(tr("Indicator"), 1, config->getIndicatorPlugins(), 0);
  idialog->addTextItem(tr("Name"), 1, tr("New Indicator"));
  idialog->addCheckItem(tr("Create Tab"), 1, TRUE);
  
  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }
  
  QString name = idialog->getText(tr("Name"));
  QString indicator = idialog->getCombo(tr("Indicator"));
  bool createTab = idialog->getCheck(tr("Create Tab"));
  delete idialog;
  
  if (! name.length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }

  QDir dir;
  if (dir.exists(config->getData(Config::IndicatorPath) + "/" + name))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, indicator);
  if (! plug)
  {
    qDebug("QtstalkerApp::slotNewIndicator - could not open plugin");
    config->closePlugin(indicator);
    return;
  }

  rc = plug->indicatorPrefDialog();
  
  if (rc)
  {
    QString s = config->getData(Config::IndicatorPath) + "/" + name;
    plug->saveIndicatorSettings(s);
    
    addIndicatorButton(name, createTab);

    if (createTab == FALSE)
    {
      QStringList l = QStringList::split(",", config->getData(Config::StackedIndicator), FALSE);
      l.append(name);
      config->setData(Config::StackedIndicator, l.join(","));
    }

    Indicator *i = new Indicator;
    i->setFile(s);
    i->setName(name);
    i->setType(indicator);
    loadIndicator(i);

    if (i->getMainPlot())
      mainPlot->draw();

    emit signalIndicatorPageRefresh();
  }

  config->closePlugin(indicator);
}

void QtstalkerApp::slotEditIndicator (QString selection, Plot *plot)
{
  Setting *set = config->getIndicator(selection);
  if (! set->count())
  {
    delete set;
    qDebug("QtstalkerApp::slotEditIndicator:indicator settings empty");
  }
  
  QString type = set->getData("plugin");
  delete set;
  
  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, type);
  if (! plug)
  {
    qDebug("QtstalkerApp::slotEditIndicator - could not open plugin");
    config->closePlugin(type);
    return;
  }

  QString s = config->getData(Config::IndicatorPath) + "/" + selection;
  plug->loadIndicatorSettings(s);
  int rc = plug->indicatorPrefDialog();

  if (rc)
  {
    plug->saveIndicatorSettings(s);
    
    Indicator *i = new Indicator;
    i->setFile(s);
    i->setName(selection);
    i->setType(type);
    loadIndicator(i);

    plot->draw();
  }

  config->closePlugin(type);
}

void QtstalkerApp::slotDeleteIndicator (QString text, Plot *plot)
{
  if (! plot->getMainFlag())
  {
    if (tabs->count() == 1)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Must leave at least one indicator tab."));
      return;
    }
  }

  // delete any chart objects that belong to the indicator
  if (! plot->getMainFlag())
  {
    QStringList l = plot->getChartObjects();
    
    ChartDb *db = new ChartDb;
    db->openChart(chartPath);
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      db->deleteChartObject(l[loop]);
    delete db;
  }

  if (! plot->getMainFlag())
  {
    if (plot->getTabFlag())
      tabs->removePage((QWidget *) plot);
    else
    {
      QStringList l = QStringList::split(",", config->getData(Config::StackedIndicator), FALSE);
      l.remove(text);
      config->setData(Config::StackedIndicator, l.join(","));
      plotList.remove(text);
    }
  }
  else
    plot->deleteIndicator(text);

  config->deleteIndicator(text);

  emit signalIndicatorPageRefresh();

  if (plot->getMainFlag())
    mainPlot->draw();
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  emit signalPixelspace(d);
  
  setSliderStart();
  
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
}

void QtstalkerApp::slotMinPixelspaceChanged (int d)
{
  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(d, 99);
  pixelspace->setValue(d);
  pixelspace->blockSignals(FALSE);
  slotPixelspaceChanged(d);
}

void QtstalkerApp::slotSliderChanged (int v)
{
  if (status == None)
    return;

  emit signalIndex(v);

  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
}

void QtstalkerApp::addIndicatorButton (QString d, bool tabFlag)
{
  Setting *set = config->getIndicator(d);
  if (! set->count())
  {
    delete set;
    qDebug("QtstalkerApp::addIndicatorButton:indicator settings empty");
  }
  
  QString type = set->getData("plugin");
  delete set;

  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, type);
  if (! plug)
  {
    qDebug("QtstalkerApp::addIndicatorButton:could not open plugin");
    config->closePlugin(type);
    return;
  }

  bool plotFlag = plug->getPlotFlag();
  config->closePlugin(type);
  
  if (plotFlag)
    return;

  Plot *plot;
  if (tabFlag)
    plot = new Plot (baseWidget);
  else
  {
    plot = new Plot (split);
    plot->setTabFlag(FALSE);
    split->moveToLast(tabs);
    plot->show();

    QValueList<int> a = split->sizes();
    a[a.size() - 2] = 75;
    split->setSizes(a);
  }
  plotList.replace(d, plot);

  QObject::connect(plot, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  QObject::connect(plot, SIGNAL(signalEditIndicator(QString, Plot *)), this, SLOT(slotEditIndicator(QString, Plot *)));
  QObject::connect(plot, SIGNAL(signalDeleteIndicator(QString, Plot *)), this, SLOT(slotDeleteIndicator(QString, Plot *)));
  QObject::connect(plot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(plot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  
  QObject::connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  QObject::connect(this, SIGNAL(signalDrawMode(bool)), plot, SLOT(setDrawMode(bool)));

  QObject::connect(this, SIGNAL(signalInterval(ChartDb::BarCompression)), plot, SLOT(setInterval(ChartDb::BarCompression)));
  compressionChanged();

  QObject::connect(this, SIGNAL(signalGrid(bool)), plot, SLOT(setGridFlag(bool)));
  emit signalGrid(config->getData(Config::Grid).toInt());

  QObject::connect(this, SIGNAL(signalScaleToScreen(bool)), plot, SLOT(setScaleToScreen(bool)));
  emit signalScaleToScreen(config->getData(Config::ScaleToScreen).toInt());

  QObject::connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  emit signalPixelspace(pixelspace->value());

  QColor color;
  color.setNamedColor(config->getData(Config::BackgroundColor));
  QObject::connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  emit signalBackgroundColor(color);

  color.setNamedColor(config->getData(Config::BorderColor));
  QObject::connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  emit signalBorderColor(color);

  color.setNamedColor(config->getData(Config::GridColor));
  QObject::connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  emit signalGridColor(color);

  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  emit signalPlotFont(font);

  plot->setDateFlag(actionPlotDate->isOn());
  QObject::connect(this, SIGNAL(signalPlotDate(bool)), plot, SLOT(setDateFlag(bool)));

  // setup the crosshair signals
  QObject::connect(plot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  QObject::connect(plot, SIGNAL(signalCrosshairsStatus(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  QObject::connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot, SLOT(setCrosshairsStatus(bool)));

  // setup plot key presses
  QObject::connect(plot, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotPlotKeyPressed(QKeyEvent *)));

  if (tabFlag)
    tabs->addTab(plot, d);
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->refreshList();
  
  if (status == None)
    return;
  
  loadChart(chartPath);

// FIXME: segfaults on a rapid quote update
//  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusBar()->message(d);
  qApp->processEvents();
}

void QtstalkerApp::slotTabChanged (QWidget *)
{
  if (plotList.count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }
}

void QtstalkerApp::initGroupNav ()
{
  GroupPage *gp = new GroupPage(baseWidget, config);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  navTab->addTab(gp, QIconSet(QPixmap(dirclosed)), QString::null);
  navTab->setTabToolTip(gp, tr("Workwith Groups"));
}

void QtstalkerApp::initChartNav ()
{
  chartNav = new ChartPage(baseWidget, config);
  connect(chartNav, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  navTab->addTab(chartNav, QIconSet(QPixmap(plainitem)), QString::null);
  navTab->setTabToolTip(chartNav, tr("Workwith Charts"));
}

void QtstalkerApp::initPortfolioNav ()
{
  PortfolioPage *pp = new PortfolioPage(baseWidget, config);
  navTab->addTab(pp, QIconSet(QPixmap(portfolio)), QString::null);
  navTab->setTabToolTip(pp, tr("Workwith Portfolios"));
}

void QtstalkerApp::initTestNav ()
{
  TestPage *tp = new TestPage(baseWidget, config);
  navTab->addTab(tp, QIconSet(QPixmap(test)), QString::null);
  navTab->setTabToolTip(tp, tr("Workwith Backtests"));
}

void QtstalkerApp::initIndicatorNav ()
{
  IndicatorPage *ip = new IndicatorPage(baseWidget, config, mainPlot);
  navTab->addTab(ip, QIconSet(QPixmap(indicator)), QString::null);
  navTab->setTabToolTip(ip, tr("Workwith Main Chart Indicators"));
  connect(this, SIGNAL(signalIndicatorPageRefresh()), ip, SLOT(refreshList()));
}

void QtstalkerApp::initScannerNav ()
{
/*
  ScannerPage *sp = new ScannerPage(baseWidget, config);
  navTab->addTab(sp, QIconSet(QPixmap(scanner)), QString::null);
  navTab->setTabToolTip(sp, tr("Workwith Scanners"));
*/
}

void QtstalkerApp::slotHideNav (bool d)
{
  actionNav->setOn(d);

  if (d)
    navBase->show();
  else
    navBase->hide();
}

void QtstalkerApp::slotUpdateInfo (Setting *r)
{
  QStringList l = r->getKeyList();
  l.sort();
  int loop;
  QString s;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    s.append(l[loop]);
    s.append(" ");
    s.append(r->getData(l[loop]));
    s.append("\n");
  }

  delete r;

  infoLabel->setText(s);
}

void QtstalkerApp::slotHideMainPlot (bool d)
{
  actionHideMainPlot->setOn(d);
  mainPlot->setHideMainPlot(d);
  mainPlot->draw();
}

void QtstalkerApp::slotPlotDate (bool d)
{
  actionPlotDate->setOn(d);
  emit signalPlotDate (d);

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
}

void QtstalkerApp::slotPlotLeftMouseButton (int x, int y, bool mainFlag)
{
  if (! mainFlag)
    mainPlot->crossHair(x, y);

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->crossHair(x, y);
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
}

void QtstalkerApp::slotPlotKeyPressed (QKeyEvent *key)
{
  if (status == None)
    return;

  switch (key->key())
  {
    case Qt::Key_Left:
      slider->setValue(slider->value() - 1);
      break;
    case Qt::Key_Right:
      slider->setValue(slider->value() + 1);
      break;
    case Qt::Key_Home:
      slider->setValue(0);
      break;
    case Qt::Key_End:
      slider->setValue(slider->maxValue());
      break;
    case Qt::Key_Prior:
      slider->addStep();
      break;
    case Qt::Key_Next:
      slider->subtractStep();
      break;
    case Qt::Key_Minus:
      pixelspace->stepDown();
      break;
    case Qt::Key_Plus:
      pixelspace->stepUp();
      break;
    case Qt::Key_Up:
      if (compressionCombo->currentItem() != (compressionCombo->count() - 1))
      {
        compressionCombo->setCurrentItem(compressionCombo->currentItem() + 1);
        slotCompressionChanged(compressionCombo->currentItem());
      }
      break;
    case Qt::Key_Down:
      if (compressionCombo->currentItem() != 0)
      {
        compressionCombo->setCurrentItem(compressionCombo->currentItem() - 1);
        slotCompressionChanged(compressionCombo->currentItem());
      }
      break;
    default:
      break;
  }
}

void QtstalkerApp::slotDrawMode (bool d)
{
  emit signalDrawMode(d);
}

void QtstalkerApp::setSliderStart ()
{
  if (status == None)
    return;

  int page = mainPlot->getWidth() / pixelspace->value();
  int max = recordList->count() - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, recordList->count() - 1);
  
  if ((int) recordList->count() < page)
  {
    slider->setValue(0);
    emit signalIndex(0);
  }
  else
  {
    slider->setValue(max + 1);
    emit signalIndex(max + 1);
  }
  
  slider->blockSignals(FALSE);
}

//**********************************************************************
//**********************************************************************
//**********************************************************************

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QTranslator tor( 0 );

  // set the location where your .qm files are in load() below as the last parameter instead of "."
  // for development, use "/" to use the english original as
  // .qm files are stored in the base project directory.
  tor.load(QString("qtstalker.") + QTextCodec::locale(), "." );
  a.installTranslator( &tor );

  QtstalkerApp *qtstalker = new QtstalkerApp();
  a.setMainWidget(qtstalker);

  qtstalker->show();

  return a.exec();
}





