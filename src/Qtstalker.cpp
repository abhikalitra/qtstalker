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
#include "EditDialog.h"
#include "ChartDb.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "IndicatorPage.h"
#include "NewIndicatorDialog.h"
#include "PlotLine.h"
//#include "ScannerPage.h"

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

  color.setNamedColor(config->getData(Config::UpColor));
  QObject::connect(this, SIGNAL(signalUpColor(QColor)), mainPlot, SLOT(setUpColor(QColor)));
  emit signalUpColor(color);

  color.setNamedColor(config->getData(Config::DownColor));
  QObject::connect(this, SIGNAL(signalDownColor(QColor)), mainPlot, SLOT(setDownColor(QColor)));
  emit signalDownColor(color);

  color.setNamedColor(config->getData(Config::NeutralColor));
  QObject::connect(this, SIGNAL(signalNeutralColor(QColor)), mainPlot, SLOT(setNeutralColor(QColor)));
  emit signalNeutralColor(color);

  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), mainPlot, SLOT(setPlotFont(QFont)));
  emit signalPlotFont(font);

  QObject::connect(mainPlot, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  QObject::connect(mainPlot, SIGNAL(signalNewChartObject(QString, Plot *)), this, SLOT(slotNewChartObject(QString, Plot *)));
  QObject::connect(mainPlot, SIGNAL(signalEditChartObject(Setting *, Plot *)), this, SLOT(slotEditChartObject(Setting *, Plot *)));
  QObject::connect(mainPlot, SIGNAL(signalDeleteChartObject(QString, Plot *)), this, SLOT(slotDeleteChartObject(QString, Plot *)));
  QObject::connect(mainPlot, SIGNAL(signalEditIndicator(QString, Plot *)), this, SLOT(slotEditIndicator(QString, Plot *)));
  QObject::connect(mainPlot, SIGNAL(signalDeleteIndicator(QString, Plot *)), this, SLOT(slotDeleteIndicator(QString, Plot *)));

  QObject::connect(mainPlot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(mainPlot, SIGNAL(chartObjectCreated(Setting *)), this, SLOT(slotChartObjectCreated(Setting *)));
  QObject::connect(mainPlot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  QObject::connect(mainPlot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  QObject::connect(mainPlot, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotPlotKeyPressed(QKeyEvent *)));

  QObject::connect(this, SIGNAL(signalGrid(bool)), mainPlot, SLOT(setGridFlag(bool)));
  QObject::connect(this, SIGNAL(signalScaleToScreen(bool)), mainPlot, SLOT(setScaleToScreen(bool)));
  QObject::connect(this, SIGNAL(signalPixelspace(int)), mainPlot, SLOT(setPixelspace(int)));
  QObject::connect(this, SIGNAL(signalIndex(int)), mainPlot, SLOT(setIndex(int)));
  QObject::connect(this, SIGNAL(signalInterval(Plot::TimeInterval)), mainPlot, SLOT(setInterval(Plot::TimeInterval)));

  tabs = new QTabWidget(split);
  if (config->getData(Config::IndicatorTabs).toInt() == 0)
    tabs->setTabPosition(QTabWidget::Bottom);
  QObject::connect(tabs, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged(QWidget *)));

  initToolBar();

  statusBar()->message(tr("Ready"), 2000);

  // set up the bar intervals
  barCombo->insertItem(tr("3 Months"), -1);
  barCombo->insertItem(tr("6 Months"), -1);
  barCombo->insertItem(tr("9 Months"), -1);
  barCombo->insertItem(tr("1 Year"), -1);
  barCombo->insertItem(tr("2 Years"), -1);
  barCombo->insertItem(tr("5 Years"), -1);
  barCombo->insertItem(tr("All"), -1);

  int loop;
  for (loop = 0; loop < (int) barCombo->count(); loop++)
  {
    QString s = barCombo->text(loop);
    if (! s.compare(config->getData(Config::Bars)))
    {
      barCombo->setCurrentItem(loop);
      break;
    }
  }
  connect (barCombo, SIGNAL(activated(int)), this, SLOT(slotBarComboChanged(int)));
  barCombo->setEnabled(FALSE);

  // setup the compression buttons
  for (loop = 0; loop < (int) compressionCombo->count(); loop++)
  {
    QString s = compressionCombo->text(loop);
    if (! s.compare(config->getData(Config::Compression)))
    {
      compressionCombo->setCurrentItem(loop);
      break;
    }
  }
  compressionChanged(config->getData(Config::Compression));

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

  slotChartTypeChanged(0);

  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initPortfolioNav();
  initTestNav();
//  initScannerNav();

  resize(config->getData(Config::Width).toInt(), config->getData(Config::Height).toInt());
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initActions()
{
  QPixmap icon(finished);
  actionQuit = new QAction(tr("Exit"), icon, tr("Exit"), 0, this);
  actionQuit->setStatusTip(tr("Quit Qtstalker."));
  connect(actionQuit, SIGNAL(activated()), this, SLOT(slotQuit()));

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

//  QAction *action = new QAction(QString::null, icon, QString::null, CTRL+Key_M, this);
//  connect(action, SIGNAL(activated()), this, SLOT(slotMainPlotFocus()));

//  action = new QAction(QString::null, icon, QString::null, CTRL+Key_C, this);
//  connect(action, SIGNAL(activated()), this, SLOT(slotChartTabFocus()));
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
  actionNewIndicator->addTo(toolbar);
  actionDatawindow->addTo(toolbar);
  actionQuotes->addTo(toolbar);

  toolbar2 = new QToolBar(this, "toolbar2");

  compressionCombo = new QComboBox(toolbar2);
  compressionCombo->show();
  QStringList l;
  l.append(tr("Daily"));
  l.append(tr("Weekly"));
  l.append(tr("Monthly"));
  compressionCombo->insertStringList(l, -1);
  QToolTip::add(compressionCombo, tr("Chart Compression"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(slotCompressionChanged(int)));

  chartTypeCombo = new QComboBox(toolbar2);
  chartTypeCombo->show();
  l.clear();
  l.append(tr("Bar"));
  l.append(tr("Paint Bar"));
  l.append(tr("Line"));
  l.append(tr("Candle"));
  l.append(tr("P&F"));
  l.append(tr("Swing"));
  chartTypeCombo->insertStringList(l, -1);
  QToolTip::add(chartTypeCombo, tr("Chart Type"));
  connect(chartTypeCombo, SIGNAL(activated(int)), this, SLOT(slotChartTypeChanged(int)));

  pixelspace = new QSpinBox(toolbar2);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));

  barCombo = new QComboBox(toolbar2);
  QToolTip::add(barCombo, tr("Bars"));

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
  QValueList<int> list = split->sizes();
  config->setData(Config::MainPlotSize, QString::number(list[0]));
  config->setData(Config::IndicatorPlotSize, QString::number(list[1]));
  config->setData(Config::Height, QString::number(this->height()));
  config->setData(Config::Width, QString::number(this->width()));
  config->setData(Config::X, QString::number(this->x()));
  config->setData(Config::Y, QString::number(this->y()));
  delete config;

  if (recordList)
    delete recordList;

  qApp->quit();
}

void QtstalkerApp::slotAbout()
{
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					tr("Qtstalker\nVersion 0.24 \n(C) 2001-2004 by Stefan Stratigakos"),
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
  barCombo->setEnabled(TRUE);
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
  EditDialog *dialog = new EditDialog(config);
  dialog->setCaption(tr("Edit Preferences"));

  Setting *set = new Setting();
  set->set(tr("Color Background"), config->getData(Config::BackgroundColor), Setting::Color);
  set->set(tr("Color Border"), config->getData(Config::BorderColor), Setting::Color);
  set->set(tr("Color Grid"), config->getData(Config::GridColor), Setting::Color);
  set->set(tr("Color Up"), config->getData(Config::UpColor), Setting::Color);
  set->set(tr("Color Down"), config->getData(Config::DownColor), Setting::Color);
  set->set(tr("Color Neutral"), config->getData(Config::NeutralColor), Setting::Color);
  set->set(tr("Paint Bar Indicator"), config->getData(Config::PaintBarIndicator), Setting::List);
  set->setList(tr("Paint Bar Indicator"), config->getIndicators());
  set->set(tr("Plot Font"), config->getData(Config::PlotFont), Setting::Font);
  set->set(tr("App Font"), config->getData(Config::AppFont), Setting::Font);
  set->set(tr("P&F Box Size"), config->getData(Config::PAFBoxSize), Setting::Float);
  set->set(tr("P&F Reversal"), config->getData(Config::PAFReversal), Setting::Integer);

  if (config->getData(Config::NavigatorPosition).toInt() == 1)
    set->set(tr("Navigator Left"), tr("True"), Setting::Bool);
  else
    set->set(tr("Navigator Left"), tr("False"), Setting::Bool);

  if (config->getData(Config::IndicatorTabs).toInt() == 1)
    set->set(tr("Indicator Tabs Top"), tr("True"), Setting::Bool);
  else
    set->set(tr("Indicator Tabs Top"), tr("False"), Setting::Bool);

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    QColor color;

    config->setData(Config::BackgroundColor, set->getData(tr("Color Background")));
    color.setNamedColor(set->getData(tr("Color Background")));
    emit signalBackgroundColor(color);

    config->setData(Config::BorderColor, set->getData(tr("Color Border")));
    color.setNamedColor(set->getData(tr("Color Border")));
    emit signalBorderColor(color);

    config->setData(Config::GridColor, set->getData(tr("Color Grid")));
    color.setNamedColor(set->getData(tr("Color Grid")));
    emit signalGridColor(color);

    config->setData(Config::UpColor, set->getData(tr("Color Up")));
    color.setNamedColor(set->getData(tr("Color Up")));
    emit signalUpColor(color);

    config->setData(Config::DownColor, set->getData(tr("Color Down")));
    color.setNamedColor(set->getData(tr("Color Down")));
    emit signalDownColor(color);

    config->setData(Config::NeutralColor, set->getData(tr("Color Neutral")));
    color.setNamedColor(set->getData(tr("Color Neutral")));
    emit signalNeutralColor(color);

    config->setData(Config::PlotFont, set->getData(tr("Plot Font")));
    QStringList l = QStringList::split(" ", set->getData(tr("Plot Font")), FALSE);
    QFont font(l[0], l[1].toInt(), l[2].toInt());
    emit signalPlotFont(font);

    config->setData(Config::AppFont, set->getData(tr("App Font")));
    l = QStringList::split(" ", set->getData(tr("App Font")), FALSE);
    QFont font2(l[0], l[1].toInt(), l[2].toInt());
    qApp->setFont(font2, TRUE, 0);

    config->setData(Config::PaintBarIndicator, set->getData(tr("Paint Bar Indicator")));

    if (! set->getData(tr("Navigator Left")).compare(tr("True")))
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

    if (! set->getData(tr("Indicator Tabs Top")).compare(tr("True")))
    {
      config->setData(Config::IndicatorTabs, "1");
      tabs->setTabPosition(QTabWidget::Top);
    }
    else
    {
      config->setData(Config::IndicatorTabs, "0");
      tabs->setTabPosition(QTabWidget::Bottom);
    }

    config->setData(Config::PAFBoxSize, set->getData(tr("P&F Box Size")));
    mainPlot->setPAFBoxSize(set->getFloat(tr("P&F Box Size")));

    config->setData(Config::PAFReversal, set->getData(tr("P&F Reversal")));
    mainPlot->setPAFReversal(set->getInt(tr("P&F Reversal")));

    loadChart(chartPath);

    statusBar()->message (tr("Preferences saved."));
  }

  delete set;
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

  ChartDb *db = new ChartDb();
  if (db->openChart(chartPath))
  {
    qDebug(tr("Unable to open chart."));
    delete db;
    return;
  }

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

  Setting *set = db->getDetails();
  chartName = set->getData(tr("Title"));
  chartType = set->getData("Chart Type");
  chartSymbol = set->getData("Symbol");
  QDateTime fd = QDateTime::fromString(set->getDateTime("First Date"), Qt::ISODate);
  QDateTime date = QDateTime::fromString(set->getDateTime("Last Date"), Qt::ISODate);

  bool otherFlag = FALSE;
  if (chartType.compare(tr("Stock"))
      && chartType.compare(tr("Futures"))
      && chartType.compare(tr("Index"))
      && chartType.compare(tr("Spread")))
  {
    chartTypeCombo->setEnabled(FALSE);
    otherFlag = TRUE;
  }
  else
    chartTypeCombo->setEnabled(TRUE);

  if (! date.isValid())
  {
    date = QDateTime::currentDateTime();
    date.setTime(QTime(0, 0, 0, 0));
  }

  switch (barCombo->currentItem())
  {
    case 0:
      date = date.addDays(-91);
      break;
    case 1:
      date = date.addDays(-183);
      break;
    case 2:
      date = date.addDays(-275);
      break;
    case 4:
      date = date.addDays(-730);
      break;
    case 5:
      date = date.addDays(-1825);
      break;
    case 6:
      if (fd.isValid())
        date = fd;
      break;
    default:
      date = date.addDays(-365);
      break;
  }

  QString compression = config->getData(Config::Compression);
  Plot::TimeInterval pi = Plot::Daily;
  ChartDb::Compression ci = ChartDb::Daily;
  if (! compression.compare(QObject::tr("Weekly")))
  {
    pi = Plot::Weekly;
    ci = ChartDb::Weekly;
  }
  else
  {
    if (! compression.compare(QObject::tr("Monthly")))
    {
      pi = Plot::Monthly;
      ci = ChartDb::Monthly;
    }
  }
  if (recordList)
    delete recordList;
  recordList = db->getHistory(ci, date);

  mainPlot->setData(recordList);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);

  l = config->getIndicators();

  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = new Indicator;
    i->parse(config->getIndicator(l[loop]));
    loadIndicator(i);

    if (i->getMainPlot() && otherFlag)
      i->clearLines();

    if (i->getMainPlot())
    {
      // restore the enable status of this main chart indicator
      if (di.findIndex(l[loop]) != -1)
	i->setEnable(FALSE);
    }
  }

  if (recordList->count())
  {
    Setting *r = recordList->at(0);
    QStringList keys = r->getKeyList();
    keys.remove("Date");
    keys.remove("Open");
    keys.remove("High");
    keys.remove("Low");
    keys.remove("Close");
    keys.remove("Volume");
    keys.remove("Open Interest");

    Indicator *i = mainPlot->getIndicator("Main Plot");

    int loop;
    for (loop = 0; loop < (int) keys.count(); loop++)
    {
      PlotLine *pl = new PlotLine;
      int loop2;
      for (loop2 = 0; loop2 < (int) recordList->count(); loop2++)
      {
        r = recordList->at(loop2);
	pl->append(r->getFloat(keys[loop]));
      }

      QString s = keys[loop];
      s.append(tr(" Color"));
      pl->setColor(set->getData(s));

      s = keys[loop];
      s.append(tr(" Line Type"));
      pl->setType(set->getData(s));

      pl->setLabel(keys[loop]);

      i->addLine(pl);
    }
  }

  l = db->getChartObjects();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *co = db->getChartObject(l[loop]);
    Indicator *i = mainPlot->getIndicator(co->getData(tr("Plot")));
    if (! i)
    {
      Plot *plot = plotList[co->getData(tr("Plot"))];
      i = plot->getIndicator(co->getData(tr("Plot")));
    }

    if (i)
      i->addChartObject(co);
    else
      delete co;
  }

  int page = mainPlot->getWidth() / mainPlot->getPixelspace();
  int max = recordList->count() - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, recordList->count() - 1);
  if (! reload)
  {
    if ((int) recordList->count() < page)
      slider->setValue(0);
    else
      slider->setValue(max + 1);
  }
  slider->blockSignals(FALSE);

  mainPlot->setIndex(slider->value());

  for(it.toFirst(); it.current(); ++it)
    it.current()->setIndex(slider->value());

  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->blockSignals(FALSE);

  mainPlot->draw();

  for(it.toFirst(); it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);

  setCaption(getWindowCaption());

  emit signalIndicatorPageRefresh();

  config->closePlugins();

  delete db;
}

void QtstalkerApp::loadIndicator (Indicator *i)
{
  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, i->getData(QObject::tr("Type")));
  if (plug)
  {
    plug->setIndicatorInput(recordList);
    plug->parse(i->getString());
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
    QString s = config->getData(Config::PaintBarIndicator);
    if (! s.compare(i->getData("Name")))
    {
      plug->getAlerts();
      mainPlot->setPaintBars(plug->getColorBars(config->getData(Config::UpColor),
      						config->getData(Config::DownColor),
						config->getData(Config::NeutralColor)));
    }

    if (i->getMainPlot())
      mainPlot->addIndicator(i->getData("Name"), i);
    else
    {
      Plot *plot = plotList[i->getData("Name")];
      plot->addIndicator(i->getData("Name"), i);
      plot->setData(recordList);
    }

    config->closePlugins();
  }
}

void QtstalkerApp::slotBarComboChanged (int index)
{
  config->setData(Config::Bars, barCombo->text(index));
  loadChart(chartPath);
}

void QtstalkerApp::slotNewChartObject (QString selection, Plot *plot)
{
  bool ok = FALSE;
  QString name = QInputDialog::getText(tr("Chart Object Name"),
  				       tr("Enter a unique name for this chart object."),
				       QLineEdit::Normal,
				       tr("New Chart Object"),
				       &ok,
				       this);
  if (ok == FALSE)
    return;

  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  QStringList l = db->getChartObjects();
  delete db;
  if (l.findIndex(name) != -1)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate chart object name."));
    return;
  }

  plot->createChartObject(selection, name);
}

void QtstalkerApp::slotChartObjectCreated (Setting *co)
{
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  db->setChartObject(co->getData("Name"), co);
  delete db;
}

void QtstalkerApp::slotEditChartObject (Setting *co, Plot *plot)
{
  EditDialog *dialog = new EditDialog(config);

  QString s = tr("Edit Chart Object");
  s.append(" - ");
  s.append(co->getData(tr("Type")));
  dialog->setCaption(s);

  dialog->setItems(co);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    ChartDb *db = new ChartDb();
    db->openChart(chartPath);
    db->setChartObject(co->getData("Name"), co);
    delete db;

    plot->draw();
  }

  delete dialog;
}

void QtstalkerApp::slotDeleteChartObject (QString name, Plot *plot)
{
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  db->deleteChartObject(name);
  delete db;
  plot->draw();
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

  caption.append(config->getData(Config::Compression));

  return caption;
}

void QtstalkerApp::slotDataWindow ()
{
  DataWindow *dw = new DataWindow(0, recordList->count());
  dw->setCaption(getWindowCaption());

  dw->setHeader(0, QObject::tr("Date"));
  int loop;
  for (loop = 0; loop < (int) recordList->count(); loop++)
  {
    Setting *r = recordList->at(loop);
    QDateTime dt = QDateTime::fromString(r->getDateTime("Date"), Qt::ISODate);
    dw->setData(loop, 0, dt.toString("yyyyMMdd"));
  }

  int col = 1;
  Indicator *i = mainPlot->getIndicator("Main Plot");
  if (! i->getLines())
  {
    dw->setHeader(1, QObject::tr("Open"));
    dw->setHeader(2, QObject::tr("High"));
    dw->setHeader(3, QObject::tr("Low"));
    dw->setHeader(4, QObject::tr("Close"));

    for (loop = 0; loop < (int) recordList->count(); loop++)
    {
      Setting *r = recordList->at(loop);
      dw->setData(loop, 1, r->getData("Open"));
      dw->setData(loop, 2, r->getData("High"));
      dw->setData(loop, 3, r->getData("Low"));
      dw->setData(loop, 4, r->getData("Close"));
    }

    col = 5;
  }

  QStringList l = mainPlot->getIndicators();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    i = mainPlot->getIndicator(l[loop]);
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
      i = plot->getIndicator(l[loop]);
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
  compressionChanged(compressionCombo->currentText());
  loadChart(chartPath);
}

void QtstalkerApp::compressionChanged (QString d)
{
  config->setData(Config::Compression, d);

  Plot::TimeInterval t;

  if (! d.compare(tr("Daily")))
    t = Plot::Daily;
  else
  {
    if (! d.compare(tr("Weekly")))
      t = Plot::Weekly;
    else
      t = Plot::Monthly;
  }

  emit signalInterval(t);
}

void QtstalkerApp::slotChartTypeChanged (int)
{
  config->setData(Config::ChartStyle, chartTypeCombo->currentText());
  mainPlot->setChartType(chartTypeCombo->currentText());

  emit signalPixelspace(mainPlot->getPixelspace());

  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->blockSignals(FALSE);

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
  NewIndicatorDialog *idialog = new NewIndicatorDialog(config);
  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }

  EditDialog *dialog = new EditDialog(config);

  dialog->setCaption(tr("Edit Indicator"));

  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, idialog->getIndicator());
  if (! plug)
  {
    delete idialog;
    delete dialog;
    qDebug("QtstalkerApp::slotNewIndicator - could not open plugin");
    return;
  }
  Setting *set = plug->getPluginSettings();
  set->set("Name", idialog->getName(), Setting::None);

  dialog->setItems(set);

  rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setIndicator(idialog->getName(), set->getStringList());
    addIndicatorButton(idialog->getName(), idialog->getTabFlag());

    if (idialog->getTabFlag() == FALSE)
    {
      QStringList l = QStringList::split(",", config->getData(Config::StackedIndicator), FALSE);
      l.append(idialog->getName());
      config->setData(Config::StackedIndicator, l.join(","));
    }

    Indicator *i = new Indicator;
    i->parse(config->getIndicator(idialog->getName()));
    loadIndicator(i);

    if (i->getMainPlot())
      mainPlot->draw();

    emit signalIndicatorPageRefresh();
  }

  config->closePlugins();

  delete idialog;
  delete dialog;
}

void QtstalkerApp::slotEditIndicator (QString selection, Plot *plot)
{
  EditDialog *dialog = new EditDialog(config);

  dialog->setCaption(tr("Edit Indicator"));

  Setting *set = new Setting;
  set->parse(config->getIndicator(selection));

  Plugin *plug = config->getPlugin(Config::IndicatorPluginPath, set->getData("Type"));
  if (! plug)
  {
    delete dialog;
    qDebug("QtstalkerApp::slotEditIndicator - could not open plugin");
    return;
  }

  Setting *set2 = plug->getPluginSettings();
  set2->merge(set->getStringList());
  set2->set("Name", selection, Setting::None);
  delete set;

  dialog->setItems(set2);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setIndicator(selection, set2->getStringList());

    Indicator *i = new Indicator;
    i->parse(config->getIndicator(set2->getData(tr("Name"))));
    loadIndicator(i);

    plot->draw();
  }

  config->closePlugins();

  delete dialog;
}

void QtstalkerApp::slotDeleteIndicator (QString text, Plot *plot)
{
  Indicator *i = plot->getIndicator(text);

  if (! plot->getMainFlag())
  {
    if (tabs->count() == 1)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Must leave at least one indicator tab."));
      return;
    }
  }

  // delete any chart objects that belong to the indicator
  QStringList l = i->getChartObjects();

  ChartDb *db = new ChartDb;
  db->openChart(chartPath);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    db->deleteChartObject(l[loop]);
  delete db;

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

  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);
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
  Indicator *i = new Indicator;
  i->parse(config->getIndicator(d));

  QString s = i->getData(tr("Plot"));
  if (! s.compare(tr("True")))
  {
    delete i;
    return;
  }

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
  QObject::connect(plot, SIGNAL(signalNewChartObject(QString, Plot *)), this, SLOT(slotNewChartObject(QString, Plot *)));
  QObject::connect(plot, SIGNAL(signalEditChartObject(Setting *, Plot *)), this, SLOT(slotEditChartObject(Setting *, Plot *)));
  QObject::connect(plot, SIGNAL(signalDeleteChartObject(QString, Plot *)), this, SLOT(slotDeleteChartObject(QString, Plot *)));
  QObject::connect(plot, SIGNAL(signalEditIndicator(QString, Plot *)), this, SLOT(slotEditIndicator(QString, Plot *)));
  QObject::connect(plot, SIGNAL(signalDeleteIndicator(QString, Plot *)), this, SLOT(slotDeleteIndicator(QString, Plot *)));

  QObject::connect(plot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(plot, SIGNAL(chartObjectCreated(Setting *)), this, SLOT(slotChartObjectCreated(Setting *)));
  QObject::connect(plot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));

  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));

  QObject::connect(this, SIGNAL(signalInterval(Plot::TimeInterval)), plot, SLOT(setInterval(Plot::TimeInterval)));
  compressionChanged(config->getData(Config::Compression));

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

  color.setNamedColor(config->getData(Config::UpColor));
  QObject::connect(this, SIGNAL(signalUpColor(QColor)), plot, SLOT(setUpColor(QColor)));
  emit signalUpColor(color);

  color.setNamedColor(config->getData(Config::DownColor));
  QObject::connect(this, SIGNAL(signalDownColor(QColor)), plot, SLOT(setDownColor(QColor)));
  emit signalDownColor(color);

  color.setNamedColor(config->getData(Config::NeutralColor));
  QObject::connect(this, SIGNAL(signalNeutralColor(QColor)), plot, SLOT(setNeutralColor(QColor)));
  emit signalNeutralColor(color);

  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  emit signalPlotFont(font);

  plot->setDateFlag(actionPlotDate->isOn());
  QObject::connect(this, SIGNAL(signalPlotDate(bool)), plot, SLOT(setDateFlag(bool)));

  // setup the crosshair signals
  QObject::connect(plot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));

  // setup plot key presses
  QObject::connect(plot, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotPlotKeyPressed(QKeyEvent *)));

  if (tabFlag)
    tabs->addTab(plot, d);

  delete i;
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->refreshList();

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





