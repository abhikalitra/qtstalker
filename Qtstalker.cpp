/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001,2002 by Stefan Stratigakos
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
#include <qtextstream.h>
#include <qfile.h>
#include <qsplitter.h>
#include <qapplication.h>
#include <qfont.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qcursor.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qstringlist.h>
#include <qlibrary.h>

#include "Qtstalker.h"
#include "GroupDialog.h"
#include "Quote.h"
#include "PortfolioDialog.h"
#include "DataWindow.h"
#include "EditDialog.h"
#include "Setting.h"
#include "CompositeDialog.h"
#include "ChartDb.h"
#include "WorkwithChartsDialog.h"
#include "WorkwithGroupsDialog.h"
#include "WorkwithPortfoliosDialog.h"

#include "dirclosed.xpm"
#include "next.xpm"
#include "prev.xpm"
#include "grid.xpm"
#include "datawindow.xpm"
#include "indicator.xpm"
#include "portfolio.xpm"
#include "quotes.xpm"
#include "done.xpm"
#include "configure.xpm"
#include "openchart.xpm"
#include "minuspixelspace.xpm"
#include "pluspixelspace.xpm"
#include "scaletoscreen.xpm"
#include "plugin.xpm"
#include "bar.xpm"
#include "line.xpm"
#include "paintbar.xpm"
#include "candle.xpm"
#include "point.xpm"

QtstalkerApp::QtstalkerApp()
{
  config = 0;
  chartMenu = 0;
  status = None;
  indicatorList.setAutoDelete(TRUE);
  tabList.setAutoDelete(FALSE);

  QString s(QDir::homeDirPath());
  s.append("/Qtstalker/config");
  config = new Config(s);

  initActions();

  initMenuBar();

  QWidget *w = new QWidget(this);
  setCentralWidget (w);

  QVBoxLayout *vbox = new QVBoxLayout(w);

  QSplitter *split = new QSplitter(w);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  mainPlot = new Plot (split);
  mainPlot->setDateFlag(TRUE);
  mainPlot->setMainFlag(TRUE);
  QObject::connect(mainPlot, SIGNAL(rightMouseButton()), this, SLOT(mainPlotPopupMenu()));
  QObject::connect(mainPlot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));

  tabBar = new QTabBar(split);
  tabBar->setMaximumHeight(20);
  tabBar->setMinimumHeight(20);
  QObject::connect(tabBar, SIGNAL(selected(int)), this, SLOT(slotIndicatorSelected(int)));

  indicatorPlot = new Plot (split);
  QObject::connect(indicatorPlot, SIGNAL(rightMouseButton()), this, SLOT(indicatorPlotPopupMenu()));
  QObject::connect(indicatorPlot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));

  initToolBar();

  setPlotColors();

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

  // setup the group combo
  connect (groupCombo, SIGNAL(activated(int)), this, SLOT(slotGroupComboChanged(int)));
  groupCombo->setEnabled(FALSE);

  // setup the compression buttons
  compressionChanged(config->getData(Config::Compression));

  // set the grid status
  s = config->getData(Config::Grid);
  if (s.toInt())
    actionGrid->setOn(TRUE);
  else
    actionGrid->setOn(FALSE);
  mainPlot->setGridFlag(s.toInt());
  indicatorPlot->setGridFlag(s.toInt());

  // set the scale to screen status
  s = config->getData(Config::ScaleToScreen);
  if (s.toInt())
    actionScaleToScreen->setOn(TRUE);
  else
    actionScaleToScreen->setOn(FALSE);
  mainPlot->setScaleToScreen(s.toInt());
  indicatorPlot->setScaleToScreen(s.toInt());

  // set the splitter size
  QValueList<int> sizeList;
  s = config->getData(Config::MainPlotSize);
  sizeList.append(s.toInt());
  sizeList.append(0);
  s = config->getData(Config::IndicatorPlotSize);
  sizeList.append(s.toInt());
  split->setSizes(sizeList);

  slotChartTypeChanged(actionBar);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initActions()
{
  QPixmap icon(openchart);
  actionWorkwithChart = new QAction(tr("Work with Charts..."), icon, tr("Work with Charts..."), 0, this);
  actionWorkwithChart->setStatusTip(tr("Open, create, delete, edit or rename charts."));
  connect(actionWorkwithChart, SIGNAL(activated()), this, SLOT(slotWorkwithChart()));

  icon = dirclosed;
  actionWorkwithGroup = new QAction(tr("Work with Groups..."), icon, tr("Work with Groups..."), 0, this);
  actionWorkwithGroup->setStatusTip(tr("Open, create, delete, edit or rename groups."));
  connect(actionWorkwithGroup, SIGNAL(activated()), this, SLOT(slotWorkwithGroup()));

  icon = finished;
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

  int pw = 30;
  int ph = 30;
  QPixmap pix(pw, ph);
  pix.fill(this, 0, 0);
  QPainter painter;
  painter.begin(&pix);
  painter.setPen(QColor("black"));
  QFont f("Helvetica", 14, 75, FALSE);
  painter.setFont(f);
  QFontMetrics fm(f);
  int w = fm.width("D", -1);
  painter.drawText ((pw - w) / 2, 24, "D", -1);
  painter.end();
  actionDaily = new QAction(tr("Daily Compression"), pix, tr("Daily Compression"), 0, this, 0, TRUE);
  actionDaily->setStatusTip(tr("Display daily chart data."));

  pix.fill(this, 0, 0);
  painter.begin(&pix);
  painter.setPen(QColor("black"));
  painter.setFont(f);
  w = fm.width("W", -1);
  painter.drawText ((pw - w) / 2, 24, "W", -1);
  painter.end();
  actionWeekly = new QAction(tr("Weekly Compression"), pix, tr("Weekly Compression"), 0, this, 0, TRUE);
  actionWeekly->setStatusTip(tr("Display weekly chart data."));

  pix.fill(this, 0, 0);
  painter.begin(&pix);
  painter.setPen(QColor("black"));
  painter.setFont(f);
  w = fm.width("M", -1);
  painter.drawText ((pw - w) / 2, 24, "M", -1);
  painter.end();
  actionMonthly = new QAction(tr("Monthly Compression"), pix, tr("Monthly Compression"), 0, this,  0, TRUE);
  actionMonthly->setStatusTip(tr("Display monthly chart data."));

  compressionGroupAction = new QActionGroup(this, 0, true);
  compressionGroupAction->insert(actionDaily);
  compressionGroupAction->insert(actionWeekly);
  compressionGroupAction->insert(actionMonthly);
  connect(compressionGroupAction, SIGNAL(selected(QAction *)), this, SLOT(slotCompressionChanged(QAction *)));

  // create chart types

  icon = bar;
  actionBar = new QAction(tr("Bar"), icon, tr("Bar"), 0, this, 0, true);
  actionBar->setStatusTip(tr("Bar Chart"));

  icon = paintbar;
  actionPaintBar = new QAction(tr("Paint Bar"), icon, tr("Paint Bar"), 0, this, 0, true);
  actionPaintBar->setStatusTip(tr("Paint Bar Chart"));

  icon = line;
  actionLine = new QAction(tr("Line"), icon, tr("Line"), 0, this, 0, true);
  actionLine->setStatusTip(tr("Line Chart"));

  icon = candle;
  actionCandle = new QAction(tr("Candle"), icon, tr("Candle"), 0, this, 0, true);
  actionCandle->setStatusTip(tr("Candle Chart"));

  icon = point;
  actionPoint = new QAction(tr("Point And Figure"), icon, tr("Point And Figure"), 0, this, 0, true);
  actionPoint->setStatusTip(tr("Point And Figure Chart"));

  chartTypeGroupAction = new QActionGroup(this, 0, true);
  chartTypeGroupAction->insert(actionBar);
  chartTypeGroupAction->insert(actionPaintBar);
  chartTypeGroupAction->insert(actionLine);
  chartTypeGroupAction->insert(actionCandle);
  chartTypeGroupAction->insert(actionPoint);
  actionBar->setOn(TRUE);
  connect(chartTypeGroupAction, SIGNAL(selected(QAction *)), this, SLOT(slotChartTypeChanged(QAction *)));

  icon = gridicon;
  actionGrid = new QAction(tr("Chart Grid"), icon, tr("Chart Grid"), 0, this, 0, true);
  actionGrid->setStatusTip(tr("Toggle the chart grid."));
  connect(actionGrid, SIGNAL(toggled(bool)), this, SLOT(slotGrid(bool)));

  icon = prev;
  actionBack = new QAction(tr("Previous Chart"), icon, tr("Previous Chart"), 0, this);
  actionBack->setStatusTip(tr("Display previous chart in current group."));
  connect(actionBack, SIGNAL(activated()), this, SLOT(slotBack()));
  actionBack->setEnabled(FALSE);

  icon = nextpix;
  actionNext = new QAction(tr("Next Chart"), icon, tr("Next Chart"), 0, this);
  actionNext->setStatusTip(tr("Display next chart in current group."));
  connect(actionNext, SIGNAL(activated()), this, SLOT(slotNext()));
  actionNext->setEnabled(FALSE);

  icon = quotes;
  actionQuotes = new QAction(tr("Quotes..."), icon, tr("Quotes..."), 0, this);
  actionQuotes->setStatusTip(tr("Download quotes from internet."));
  connect(actionQuotes, SIGNAL(activated()), this, SLOT(slotQuotes()));

  icon = portfolio;
  actionPortfolio = new QAction(tr("Work with Portfolios..."), icon, tr("Work with Portfolios..."), 0, this);
  actionPortfolio->setStatusTip(tr("Create, modify, delete a portfolio."));
  connect(actionPortfolio, SIGNAL(activated()), this, SLOT(slotWorkwithPortfolio()));

  icon = datawindow;
  actionDatawindow = new QAction(tr("Data Window..."), icon, tr("Data Window..."), 0, this);
  actionDatawindow->setStatusTip(tr("Show the data window."));
  connect(actionDatawindow, SIGNAL(activated()), this, SLOT(slotDataWindow()));
  actionDatawindow->setEnabled(FALSE);

  actionAbout = new QAction(tr("About"), tr("&About..."), 0, this);
  actionAbout->setStatusTip(tr("About Qtstalker."));
  connect(actionAbout, SIGNAL(activated()), this, SLOT(slotAbout()));

  icon = minuspixelspace;
  actionMinusPixelspace = new QAction(tr("Decrease Bar Spacing"), icon, tr("Decrease Bar Spacing"), 0, this);
  actionMinusPixelspace->setStatusTip(tr("Decrease the space between each data bar."));
  connect(actionMinusPixelspace, SIGNAL(activated()), this, SLOT(slotMinusPixelspace()));
  actionMinusPixelspace->setEnabled(FALSE);

  icon = pluspixelspace;
  actionPlusPixelspace = new QAction(tr("Increase Bar Spacing"), icon, tr("Increase Bar Spacing"), 0, this);
  actionPlusPixelspace->setStatusTip(tr("Increase the space between each data bar."));
  connect(actionPlusPixelspace, SIGNAL(activated()), this, SLOT(slotPlusPixelspace()));
  actionPlusPixelspace->setEnabled(FALSE);

  icon = scaletoscreen;
  actionScaleToScreen = new QAction(tr("Scale To Screen"), icon, tr("Scale To Screen"), 0, this, 0, true);
  actionScaleToScreen->setStatusTip(tr("Scale chart to current screen data."));
  connect(actionScaleToScreen, SIGNAL(toggled(bool)), this, SLOT(slotScaleToScreen(bool)));

  icon = plugin;
  actionNewPlugin = new QAction(tr("Install new plugins..."), icon, tr("Install new plugins..."), 0, this);
  actionNewPlugin->setStatusTip(tr("Install new plugins."));
  connect(actionNewPlugin, SIGNAL(activated()), this, SLOT(slotNewPlugin()));

  actionDeletePlugin = new QAction(tr("Delete plugins..."), icon, tr("Delete plugins..."), 0, this);
  actionDeletePlugin->setStatusTip(tr("Delete plugins."));
  connect(actionDeletePlugin, SIGNAL(activated()), this, SLOT(slotDeletePlugin()));
}

void QtstalkerApp::initMenuBar()
{
  fileMenu = new QPopupMenu();
  actionWorkwithChart->addTo(fileMenu);
  actionWorkwithGroup->addTo(fileMenu);
  actionPortfolio->addTo(fileMenu);
  fileMenu->insertSeparator();
  actionQuit->addTo(fileMenu);

  editMenu = new QPopupMenu();
  actionNewIndicator->addTo(editMenu);
  actionOptions->addTo(editMenu);

  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
  actionDaily->addTo(viewMenu);
  actionWeekly->addTo(viewMenu);
  actionMonthly->addTo(viewMenu);
  viewMenu->insertSeparator();
  actionBar->addTo(viewMenu);
  actionPaintBar->addTo(viewMenu);
  actionLine->addTo(viewMenu);
  actionCandle->addTo(viewMenu);
  actionPoint->addTo(viewMenu);
  viewMenu->insertSeparator();
  actionGrid->addTo(viewMenu);
  actionScaleToScreen->addTo(viewMenu);
  viewMenu->insertSeparator();
  actionBack->addTo(viewMenu);
  actionNext->addTo(viewMenu);
  viewMenu->insertSeparator();
  actionMinusPixelspace->addTo(viewMenu);
  actionPlusPixelspace->addTo(viewMenu);

  pluginMenu = new QPopupMenu();
  actionNewPlugin->addTo(pluginMenu);
  actionDeletePlugin->addTo(pluginMenu);

  toolMenu = new QPopupMenu();
  actionDatawindow->addTo(toolMenu);
  actionQuotes->addTo(toolMenu);

  helpMenu = new QPopupMenu();
  actionAbout->addTo(helpMenu);

  menuBar()->insertItem(tr("&File"), fileMenu);
  menuBar()->insertItem(tr("&Edit"), editMenu);
  menuBar()->insertItem(tr("&View"), viewMenu);
  menuBar()->insertItem(tr("&Plugins"), pluginMenu);
  menuBar()->insertItem(tr("&Tools"), toolMenu);
  menuBar()->insertSeparator();
  menuBar()->insertItem(tr("&Help"), helpMenu);
  
  chartMenu = new QPopupMenu();
  chartMenu->insertItem(tr("New Indicator"), this, SLOT(slotNewIndicator()));
  chartDeleteMenu = new QPopupMenu();
  chartEditMenu = new QPopupMenu();
  chartMenu->insertItem(tr("Edit Indicator"), chartEditMenu);
  chartMenu->insertItem (tr("Delete Indicator"), chartDeleteMenu);
  chartMenu->insertSeparator ();
  chartObjectDeleteMenu = new QPopupMenu();
  chartObjectEditMenu = new QPopupMenu();
  chartObjectId = chartMenu->insertItem(tr("New Chart Object"), this, SLOT(slotNewChartObject(int)));
  chartMenu->insertItem (tr("Edit Chart Object"), chartObjectEditMenu);
  chartMenu->insertItem (tr("Delete Chart Object"), chartObjectDeleteMenu);
}

void QtstalkerApp::initToolBar()
{
  // construct the chart toolbar
  toolbar = new QToolBar(this, "toolbar");

  actionWorkwithChart->addTo(toolbar);
  actionWorkwithGroup->addTo(toolbar);
  actionPortfolio->addTo(toolbar);
  actionNewIndicator->addTo(toolbar);

  toolbar->addSeparator();

  actionDaily->addTo(toolbar);
  actionWeekly->addTo(toolbar);
  actionMonthly->addTo(toolbar);

  toolbar->addSeparator();

  actionBar->addTo(toolbar);
  actionPaintBar->addTo(toolbar);
  actionLine->addTo(toolbar);
  actionCandle->addTo(toolbar);
  actionPoint->addTo(toolbar);

  toolbar->addSeparator();

  actionMinusPixelspace->addTo(toolbar);
  actionPlusPixelspace->addTo(toolbar);

  toolbar->addSeparator();

  actionGrid->addTo(toolbar);
  actionScaleToScreen->addTo(toolbar);

  toolbar->addSeparator();

  actionDatawindow->addTo(toolbar);
  actionQuotes->addTo(toolbar);

  // construct the navigation toolbar
  navToolbar = new QToolBar(this, "nav toolbar");

  barCombo = new QComboBox(navToolbar);

  navToolbar->addSeparator();

  groupCombo = new QComboBox(navToolbar);
  groupCombo->setMinimumWidth(125);

  actionBack->addTo(navToolbar);

  actionNext->addTo(navToolbar);

  navToolbar->addSeparator();

  slider = new QSlider(navToolbar);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  slider->setEnabled(FALSE);

  QStringList l = config->getIndicators();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    addIndicatorButton(l[loop]);
}

void QtstalkerApp::slotQuit()
{
  config->setData(Config::MainPlotSize, QString::number(mainPlot->height()));
  config->setData(Config::IndicatorPlotSize, QString::number(indicatorPlot->height()));
  delete config;

  qApp->quit();
}

void QtstalkerApp::slotAbout()
{
  QMessageBox::about(this, tr("About..."), tr("Qtstalker\nVersion 0.15 \n(C) 2001,2002 by Stefan Stratigakos"));
}

void QtstalkerApp::slotWorkwithChart ()
{
  WorkwithChartsDialog *dialog = new WorkwithChartsDialog(config);
  QObject::connect(dialog, SIGNAL(chartOpened(QString)), this, SLOT(slotOpenChart(QString)));
  dialog->show();
  slotStatusMessage(tr("Scanning symbols..."));
  dialog->updateList();
  statusBar()->message(tr("Ready"), 2000);
}

void QtstalkerApp::slotOpenChart (QString selection)
{
  slider->setEnabled(TRUE);
  actionDatawindow->setEnabled(TRUE);
  actionBack->setEnabled(FALSE);
  actionNext->setEnabled(FALSE);
  actionNewIndicator->setEnabled(TRUE);
  barCombo->setEnabled(TRUE);
  groupCombo->setEnabled(FALSE);
  actionMinusPixelspace->setEnabled(TRUE);
  actionPlusPixelspace->setEnabled(TRUE);

  groupCombo->clear();

  status = Chart;

  loadChart(selection);
}

void QtstalkerApp::slotWorkwithGroup ()
{
  WorkwithGroupsDialog *dialog = new WorkwithGroupsDialog(config);
  QObject::connect(dialog, SIGNAL(groupOpened(QString)), this, SLOT(slotOpenGroup(QString)));
  dialog->show();
  dialog->updateList();
}

void QtstalkerApp::slotOpenGroup(QString selection)
{
  slider->setEnabled(TRUE);
  actionDatawindow->setEnabled(TRUE);
  actionBack->setEnabled(TRUE);
  actionNext->setEnabled(TRUE);
  actionNewIndicator->setEnabled(TRUE);
  barCombo->setEnabled(TRUE);
  groupCombo->setEnabled(TRUE);
  actionMinusPixelspace->setEnabled(TRUE);
  actionPlusPixelspace->setEnabled(TRUE);

  groupCombo->clear();

  config->setData(Config::Group, selection);

  Setting *set = config->getGroup(selection);
  QStringList l = QStringList::split(" ", set->getData("Group"), FALSE);
  l.sort();
  groupCombo->insertStringList(l, -1);
  delete set;

  if (l.count() > 1)
  {
    actionBack->setEnabled(TRUE);
    actionNext->setEnabled(TRUE);
  }
  else
  {
    actionBack->setEnabled(FALSE);
    actionNext->setEnabled(FALSE);
  }

  status = Group;

  if (l.count())
  {
    QString s = config->getData(Config::DataPath);
    s.append("/");
    s.append(groupCombo->currentText());
    loadChart(s);
  }
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
  SettingItem *i = set->getItem(tr("Paint Bar Indicator"));
  i->list = config->getIndicators();

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setData(Config::BackgroundColor, set->getData(tr("Color Background")));
    config->setData(Config::BorderColor, set->getData(tr("Color Border")));
    config->setData(Config::GridColor, set->getData(tr("Color Grid")));
    config->setData(Config::UpColor, set->getData(tr("Color Up")));
    config->setData(Config::DownColor, set->getData(tr("Color Down")));
    config->setData(Config::NeutralColor, set->getData(tr("Color Neutral")));
    config->setData(Config::PaintBarIndicator, set->getData(tr("Paint Bar Indicator")));

    setPlotColors();

    loadChart(chartPath);

    statusBar()->message (tr("Preferences saved."));
  }

  delete set;
  delete dialog;
}

void QtstalkerApp::slotBack ()
{
  int i = groupCombo->currentItem();
  i--;
  if (i < 0)
    i = groupCombo->count() - 1;
  groupCombo->setCurrentItem(i);
  slotGroupComboChanged(i);
}

void QtstalkerApp::slotNext ()
{
  int i = groupCombo->currentItem();
  i++;
  if (i > groupCombo->count() - 1)
    i = 0;
  groupCombo->setCurrentItem(i);
  slotGroupComboChanged(i);
}

void QtstalkerApp::slotGrid (bool state)
{
  config->setData(Config::Grid, QString::number(state));

  mainPlot->setGridFlag(state);
  indicatorPlot->setGridFlag(state);

  mainPlot->draw();
  indicatorPlot->draw();
}

void QtstalkerApp::slotScaleToScreen (bool state)
{
  config->setData(Config::ScaleToScreen, QString::number(state));

  mainPlot->setScaleToScreen(state);
  indicatorPlot->setScaleToScreen(state);

  mainPlot->draw();
  indicatorPlot->draw();
}

void QtstalkerApp::loadChart (QString d)
{
  if (d.length() == 0)
    return;

  QDir dir(d);
  if (! dir.exists(d, TRUE))
    return;

  chartPath = d;

  ChartDb *db = new ChartDb();
  db->setPath(chartPath);
  if (db->openChart())
  {
    qDebug(tr("Unable to open chart."));
    delete db;
    return;
  }

  mainPlot->clear();
  mainPlot->clearDate();
  mainPlot->clearChartObjects();
  mainPlot->clearLines();
  indicatorList.clear();
  indicatorPlot->clear();
  indicatorPlot->clearDate();
  indicatorPlot->clearChartObjects();
  indicatorPlot->clearLines();

  Setting *set = db->getDetails();
  chartName = set->getData(tr("Title"));
  chartType = set->getData("Chart Type");
  chartSymbol = set->getData("Symbol");
  QDateTime fd = db->getDateTime(set->getData("First Date"));
  QDateTime date = db->getDateTime(set->getData("Last Date"));
  delete set;

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
  if (! compression.compare(QObject::tr("Daily Compression")))
  {
    db->getHistory(ChartDb::Daily, date);
    mainPlot->setInterval(Plot::Daily);
    indicatorPlot->setInterval(Plot::Daily);
  }
  else
  {
    if (! compression.compare(QObject::tr("Weekly Compression")))
    {
      db->getHistory(ChartDb::Weekly, date);
      mainPlot->setInterval(Plot::Weekly);
      indicatorPlot->setInterval(Plot::Weekly);
    }
    else
    {
      db->getHistory(ChartDb::Monthly, date);
      mainPlot->setInterval(Plot::Monthly);
      indicatorPlot->setInterval(Plot::Monthly);
    }
  }

  mainPlot->setData(db->getRecordList());
  indicatorPlot->setData(db->getRecordList());

  indicatorPlot->setPixelspace(mainPlot->getPixelspace());

  QStringList l = config->getIndicators();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = new Indicator;
    i->parse(config->getIndicator(l[loop]));

    QString s = config->getData(Config::PluginPath);
    s.append("/");
    s.append(i->getData(QObject::tr("Type")));
    s.append(".so");

    QLibrary *lib = new QLibrary(s);
    Plugin *(*so)() = 0;
    so = (Plugin *(*)()) lib->resolve("create");
    if (so)
    {
      Plugin *plug = (*so)();

      plug->setIndicatorInput(db->getRecordList());

      plug->parse(i->getString());

      plug->calculate();

      int loop2;
      for (loop2 = 0; loop2 < plug->getIndicatorLines(); loop2++)
      {
        Setting *set = plug->getIndicatorLineSettings(loop2);

	if (i->getMainPlot())
	{
	  int num = mainPlot->addLine(i->getData(set->getData("Color")),
	                              i->getData(set->getData("Line Type")),
				      i->getData(set->getData("Label")),
				      plug->getIndicatorLineArray(loop2));
	  i->addLine(num);
	}
	else
	{
	  int num = indicatorPlot->addLine(i->getData(set->getData("Color")),
	                                   i->getData(set->getData("Line Type")),
				           i->getData(set->getData("Label")),
				           plug->getIndicatorLineArray(loop2));
	  i->addLine(num);
	}

        delete set;
      }

      s = i->getData(QObject::tr("Alert"));
      if (! s.compare(QObject::tr("True")))
        i->setAlerts(plug->getAlerts());
      else
      {
        QMemArray<int> a(mainPlot->getDataSize());
	a.fill(0, -1);
	mainPlot->setAlerts(a);
      }

      delete plug;
    }

    indicatorList.insert(i->getData("Name"), i);

    delete lib;
  }

  indicatorList.insert(tr("Main Plot"), new Indicator);

  l = db->getChartObjects();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *co = db->getChartObject(l[loop]);

    QString s = co->getData(tr("Plot"));
    Indicator *i = indicatorList[s];
    int num;
    if (! s.compare(tr("Main Plot")))
      num = mainPlot->insertChartObject(co);
    else
      num = indicatorPlot->insertChartObject(co);

    i->addChartObject(l[loop], num);

    delete co;
  }

  // set up the paint bar if needed
  QString s = config->getData(Config::ChartStyle);
  if (! s.compare(tr("Paint Bar")))
  {
    QString ind = config->getData(Config::PaintBarIndicator);
    Indicator *i = indicatorList[ind];
    if (i)
      mainPlot->setAlerts(i->getAlerts());
  }

  int page = mainPlot->getWidth() / mainPlot->getPixelspace();
  int max = db->getDataSize() - page;
  if (max < 0)
    max = 0;
  slider->setRange(0, db->getDataSize() - 1);
  slider->blockSignals(TRUE);
  slider->setValue(max);
  slider->blockSignals(FALSE);

  indicatorPlot->setIndex(max);
  mainPlot->setIndex(max);

  indicatorPlot->hideLines();
  indicatorPlot->hideChartObjects();

  mainPlot->draw();

  if (tabBar->count())
    slotIndicatorSelected(tabBar->currentTab());

  setCaption(getWindowCaption());

  slotUpdateSpaceButtons();

  delete db;
}

void QtstalkerApp::slotGroupComboChanged (int)
{
  QString s = config->getData(Config::DataPath);
  s.append("/");
  s.append(groupCombo->currentText());
  loadChart(s);
}

void QtstalkerApp::slotBarComboChanged (int index)
{
  config->setData(Config::Bars, barCombo->text(index));
  loadChart(chartPath);
}

void QtstalkerApp::slotNewChartObject (int id)
{
  bool ok = FALSE;
  QString selection = QInputDialog::getItem(tr("New Chart Object"), tr("Select a new chart object to create."),
  					    mainPlot->getChartObjectList(), 0, FALSE, &ok, this);
  if (ok == FALSE)
    return;

  ok = FALSE;
  QString name = QInputDialog::getText(tr("Chart Object Name"), tr("Enter a unique name for this chart object."),
                                       QLineEdit::Normal, tr("New Chart Object"), &ok, this);
  if (ok == FALSE)
    return;

  ChartDb *db = new ChartDb();
  db->setPath(chartPath);
  db->openChart();
  QStringList l = db->getChartObjects();
  delete db;
  if (l.findIndex(name) != -1)
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate chart object name."));
    return;
  }

  EditDialog *dialog = new EditDialog(config);

  QString s = tr("New Chart Object");
  s.append(" - ");
  s.append(selection);
  dialog->setCaption(s);

  Setting *co = mainPlot->newChartObject(selection);
  co->set("Name", name, Setting::None);
  if (id)
    co->set(tr("Plot"), indicatorGroupText, Setting::None);

  dialog->setItems(co);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    ChartDb *db = new ChartDb();
    db->setPath(chartPath);
    db->openChart();
    db->setChartObject(name, co);
    delete db;

    s = co->getData(tr("Plot"));
    Indicator *i = indicatorList[s];
    if (id == 0)
    {
      int num = mainPlot->insertChartObject(co);
      i->addChartObject(name, num);
      mainPlot->draw();
    }
    else
    {
      int num = indicatorPlot->insertChartObject(co);
      i->addChartObject(name, num);
      indicatorPlot->draw();
    }
  }

  delete co;
  delete dialog;
}

void QtstalkerApp::slotEditChartObject (int id)
{
  bool flag = FALSE;
  Setting *co = mainPlot->getChartObject(id);
  if (! co)
  {
    co = indicatorPlot->getChartObject(id);
    flag = TRUE;
  }

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
    db->setPath(chartPath);
    db->openChart();
    db->setChartObject(co->getData("Name"), co);
    delete db;

    if (! flag)
      mainPlot->draw();
    else
      indicatorPlot->draw();
  }

  delete dialog;
}

void QtstalkerApp::slotDeleteChartObject (int id)
{
  bool flag = FALSE;
  Setting *co = mainPlot->getChartObject(id);
  if (! co)
  {
    co = indicatorPlot->getChartObject(id);
    flag = TRUE;
  }

  QString name = co->getData("Name");
  Indicator *i;
  if (! flag)
    i = indicatorList[tr("Main Plot")];
  else
    i = indicatorList[indicatorGroupText];
  i->deleteChartObject(name);

  ChartDb *db = new ChartDb();
  db->setPath(chartPath);
  db->openChart();
  db->deleteChartObject(name);
  delete db;

  if (! flag)
  {
    mainPlot->deleteChartObject(id);
    mainPlot->draw();
  }
  else
  {
    indicatorPlot->deleteChartObject(id);
    indicatorPlot->draw();
  }
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
    case Group:
      caption.append(": [");
      caption.append(config->getData(Config::Group));
      caption.append("] - ");
      caption.append(chartName);
      caption.append(" (");
      caption.append(chartSymbol);
      caption.append(") ");
      break;
    default:
      break;
  }

  caption.append(config->getData(Config::Compression));

  return caption;
}

void QtstalkerApp::slotWorkwithPortfolio ()
{
  WorkwithPortfoliosDialog *dialog = new WorkwithPortfoliosDialog(config);
  dialog->show();
  dialog->updateList();
}

void QtstalkerApp::slotDataWindow ()
{
  DataWindow *dw = new DataWindow(5, mainPlot->getDataSize());
  dw->setCaption(getWindowCaption());
  dw->setHeader(0, QObject::tr("Date"));
  dw->setHeader(1, QObject::tr("Open"));
  dw->setHeader(2, QObject::tr("High"));
  dw->setHeader(3, QObject::tr("Low"));
  dw->setHeader(4, QObject::tr("Close"));

  int loop;
  for (loop = 0; loop < (int) mainPlot->getDataSize(); loop++)
  {
    QDateTime dt = mainPlot->getDate(loop);
    dw->setData(loop, 0, dt.toString("yyyyMMdd"));
    dw->setData(loop, 1, QString::number(mainPlot->getOpen(loop)));
    dw->setData(loop, 2, QString::number(mainPlot->getHigh(loop)));
    dw->setData(loop, 3, QString::number(mainPlot->getLow(loop)));
    dw->setData(loop, 4, QString::number(mainPlot->getClose(loop)));
  }

  int col = 5;
  QDictIterator<Indicator> it(indicatorList);
  while (it.current())
  {
    Indicator *i = it.current();
    QMemArray<int> a = i->getLines();

    for (loop = 0; loop < (int) a.size(); loop++, col++)
    {
      QMemArray<double> d;
      if (i->getMainPlot())
      {
        dw->setHeader(col, mainPlot->getLineLabel(a[loop]));
        d = mainPlot->getLineData(a[loop]);
      }
      else
      {
        dw->setHeader(col, indicatorPlot->getLineLabel(a[loop]));
        d = indicatorPlot->getLineData(a[loop]);
      }

      int loop2;
      int offset = mainPlot->getDataSize() - d.size();
      for (loop2 = 0; loop2 < (int) d.size(); loop2++)
        dw->setData(loop2 + offset, col, mainPlot->strip(d[loop2]));
    }

    ++it;
  }

  dw->show();
}

void QtstalkerApp::plotPopupMenu (int area)
{
  chartEditMenu->clear();
  chartDeleteMenu->clear();

  int loop;
  QStringList l = config->getIndicators();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    int id = chartDeleteMenu->insertItem(l[loop], this, SLOT(slotDeleteIndicator(int)));
    chartDeleteMenu->setItemParameter(id, id);

    id = chartEditMenu->insertItem(l[loop], this, SLOT(slotEditIndicator(int)));
    chartEditMenu->setItemParameter(id, id);
  }

  chartMenu->setItemParameter(chartObjectId, area);
}

void QtstalkerApp::mainPlotPopupMenu ()
{
  plotPopupMenu(0);
  
  chartObjectEditMenu->clear();
  chartObjectDeleteMenu->clear();

  Indicator *i = indicatorList[tr("Main Plot")];
  QStringList l = i->getChartObjectNames();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    int num = i->getChartObject(l[loop]);

    int id = chartObjectEditMenu->insertItem(l[loop], this, SLOT(slotEditChartObject(int)));
    chartObjectEditMenu->setItemParameter(id, num);

    id = chartObjectDeleteMenu->insertItem(l[loop], this, SLOT(slotDeleteChartObject(int)));
    chartObjectDeleteMenu->setItemParameter(id, num);
  }

  chartMenu->exec(QCursor::pos());
}

void QtstalkerApp::indicatorPlotPopupMenu ()
{
  if (! indicatorGroupText.length())
    return;

  plotPopupMenu(1);
  
  chartObjectEditMenu->clear();
  chartObjectDeleteMenu->clear();

  Indicator *i = indicatorList[indicatorGroupText];
  QStringList l = i->getChartObjectNames();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    int num = i->getChartObject(l[loop]);

    int id = chartObjectEditMenu->insertItem(l[loop], this, SLOT(slotEditChartObject(int)));
    chartObjectEditMenu->setItemParameter(id, num);

    id = chartObjectDeleteMenu->insertItem(l[loop], this, SLOT(slotDeleteChartObject(int)));
    chartObjectDeleteMenu->setItemParameter(id, num);
  }

  chartMenu->exec(QCursor::pos());
}

void QtstalkerApp::slotCompressionChanged (QAction *action)
{
  compressionChanged(action->text());
  loadChart(chartPath);
}

void QtstalkerApp::compressionChanged (QString d)
{
  config->setData(Config::Compression, d);

  Plot::TimeInterval t;

  if (! d.compare(actionDaily->text()))
  {
    actionDaily->setOn(TRUE);
    t = Plot::Daily;
  }
  else
  {
    if (! d.compare(actionWeekly->text()))
    {
      actionWeekly->setOn(TRUE);
      t = Plot::Weekly;
    }
    else
    {
      actionMonthly->setOn(TRUE);
      t = Plot::Monthly;
    }
  }

  mainPlot->setInterval(t);
  indicatorPlot->setInterval(t);
}

void QtstalkerApp::slotChartTypeChanged (QAction *action)
{
  config->setData(Config::ChartStyle, action->text());
  mainPlot->setChartType(action->text());
  indicatorPlot->setPixelspace(mainPlot->getPixelspace());

  QString s = config->getData(Config::ChartStyle);
  if (! s.compare(tr("Paint Bar")))
  {
    QString ind = config->getData(Config::PaintBarIndicator);
    if (ind.length())
    {
      Indicator *i = indicatorList[ind];
      mainPlot->setAlerts(i->getAlerts());
    }
  }

  slotUpdateSpaceButtons();

  mainPlot->draw();
  indicatorPlot->draw();
}

void QtstalkerApp::slotNewIndicator ()
{
  bool ok;
  QStringList list = QStringList::split(",", config->getData(Config::IndicatorPlugin), FALSE);
  QString selection = QInputDialog::getItem(tr("New Indicator"), tr("Select an indicator create."),
  					    list, 0, FALSE, &ok, this);
  if (! ok || ! selection.length())
    return;

  QString name = QInputDialog::getText(tr("Indicator Name"), tr("Enter a unique name for this indicator."),
                                       QLineEdit::Normal, tr("New Indicator"), &ok, this);
  if (! ok || ! name.length())
    return;

  QStringList l = config->getIndicator(name);
  if (l.count())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  EditDialog *dialog = new EditDialog(config);

  dialog->setCaption(tr("Edit Indicator"));

  Setting *set = new Setting;
  set->set("Name", name, Setting::None);

  QString s = config->getData(Config::PluginPath);
  s.append("/");
  s.append(selection);
  s.append(".so");

  QLibrary *lib = new QLibrary(s);
  Plugin *(*so)() = 0;
  so = (Plugin *(*)()) lib->resolve("create");
  if (so)
  {
    Plugin *plug = (*so)();

    QStringList key = plug->getKeyList();

    int loop;
    for(loop = 0; loop < (int) key.count(); loop++)
      set->set(key[loop], plug->getData(key[loop]), plug->getType(key[loop]));

    delete plug;
  }

  delete lib;

  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setIndicator(name, set->getStringList());
    addIndicatorButton(name);
    loadChart(chartPath);
  }

  delete dialog;
  delete set;
}

void QtstalkerApp::slotEditIndicator (int id)
{
  QString selection = chartEditMenu->text(id);

  EditDialog *dialog = new EditDialog(config);

  dialog->setCaption(tr("Edit Indicator"));

  Setting *set = new Setting();
  set->parse(config->getIndicator(selection));
  dialog->setItems(set);

  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    config->setIndicator(set->getData(tr("Name")), set->getStringList());
    loadChart(chartPath);
  }

  delete set;
  delete dialog;
}

void QtstalkerApp::slotDeleteIndicator (int id)
{
  QString text = chartDeleteMenu->text(id);

  config->deleteIndicator(text);

  Indicator *i = indicatorList[text];

  if (! i->getMainPlot())
  {
    int loop;
    for (loop = 0; loop < (int) tabList.count(); loop++)
    {
      QTab *tab = tabList.at(loop);
      if (! text.compare(tab->text()))
      {
        tabList.remove(tab);
	tabBar->removeTab(tab);
        break;
      }
    }

    // delete any chart objects that belong to the indicator
    QStringList l = i->getChartObjectNames();

    ChartDb *db = new ChartDb;
    db->setPath(chartPath);
    db->openChart();

    for (loop = 0; loop < (int) l.count(); loop++)
      db->deleteChartObject(l[loop]);

    delete db;
  }

  loadChart(chartPath);
}

void QtstalkerApp::slotUpdateSpaceButtons ()
{
  if (mainPlot->getPixelspace() <= mainPlot->getMinPixelspace())
    actionMinusPixelspace->setEnabled(FALSE);
  else
    actionMinusPixelspace->setEnabled(TRUE);
}

void QtstalkerApp::slotMinusPixelspace ()
{
  int ps = mainPlot->getPixelspace();
  ps--;
  mainPlot->setPixelspace(ps);
  indicatorPlot->setPixelspace(ps);

  mainPlot->draw();
  indicatorPlot->draw();

  slotUpdateSpaceButtons();
}

void QtstalkerApp::slotPlusPixelspace ()
{
  int ps = mainPlot->getPixelspace();
  ps++;
  mainPlot->setPixelspace(ps);
  indicatorPlot->setPixelspace(ps);

  mainPlot->draw();
  indicatorPlot->draw();

  slotUpdateSpaceButtons();
}

void QtstalkerApp::slotSliderChanged (int v)
{
  if (status == None)
    return;

  mainPlot->setIndex(v);
  indicatorPlot->setIndex(v);

  mainPlot->draw();
  indicatorPlot->draw();
}

void QtstalkerApp::slotIndicatorSelected (int id)
{
  QTab *tab = tabBar->tab(id);
  indicatorGroupText = tab->text();

  if (status == None)
    return;

  indicatorPlot->hideLines();
  indicatorPlot->hideChartObjects();

  Indicator *i = indicatorList[indicatorGroupText];

  QMemArray<int> a = i->getLines();
  int loop;
  for (loop = 0; loop < (int) a.size(); loop++)
    indicatorPlot->showLine(a[loop]);

  a = i->getChartObjects();
  for (loop = 0; loop < (int) a.size(); loop++)
    indicatorPlot->showChartObject(a[loop]);

  indicatorPlot->draw();
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  Indicator *i = new Indicator;
  i->parse(config->getIndicator(d));

  QString s = i->getData(tr("Plot"));
  if (! s.compare(tr("True")))
  {
    delete i;
    return;
  }

  QTab *tab = new QTab(d);
  tabBar->addTab(tab);
  tabList.append(tab);
  tabBar->repaint();

  delete i;
}

void QtstalkerApp::slotNewPlugin ()
{
  QStringList l = QFileDialog::getOpenFileNames("*.so", QString::null, this, "file dialog");
  if (l.count())
  {
    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      config->installPlugin(l[loop]);
  }
}

void QtstalkerApp::slotDeletePlugin ()
{
  bool ok;

  QString s = config->getData(Config::IndicatorPlugin);
  s.append(",");
  s.append(config->getData(Config::QuotePlugin));
  QStringList l = QStringList::split(",", s, FALSE);
  l.sort();

  QString selection = QInputDialog::getItem(tr("Delete Plugin"), tr("Select a plugin to delete."),
  					    l, 0, FALSE, &ok, this);
  if (ok && ! selection.isNull())
    config->deletePlugin(selection);
}

void QtstalkerApp::slotChartUpdated ()
{
  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusBar()->message(d);
}

void QtstalkerApp::setPlotColors ()
{
  QColor color;

  color.setNamedColor(config->getData(Config::BackgroundColor));
  mainPlot->setBackgroundColor(color);
  indicatorPlot->setBackgroundColor(color);

  color.setNamedColor(config->getData(Config::BorderColor));
  mainPlot->setBorderColor(color);
  indicatorPlot->setBorderColor(color);

  color.setNamedColor(config->getData(Config::GridColor));
  mainPlot->setGridColor(color);
  indicatorPlot->setGridColor(color);

  color.setNamedColor(config->getData(Config::UpColor));
  mainPlot->setUpColor(color);
  indicatorPlot->setUpColor(color);

  color.setNamedColor(config->getData(Config::DownColor));
  mainPlot->setDownColor(color);
  indicatorPlot->setDownColor(color);

  color.setNamedColor(config->getData(Config::NeutralColor));
  mainPlot->setNeutralColor(color);
  indicatorPlot->setNeutralColor(color);
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
  tor.load( QString("qtstalker.") + QTextCodec::locale(), "." );
  a.installTranslator( &tor );

  QtstalkerApp *qtstalker = new QtstalkerApp();
  a.setMainWidget(qtstalker);

  qtstalker->show();

  return a.exec();
}





