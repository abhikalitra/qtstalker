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
#include <qapplication.h>
#include <qfont.h>
#include <qtextcodec.h>
#include <qtranslator.h>
#include <qcursor.h>
#include <qcolor.h>
#include <qfontmetrics.h>
#include <qstringlist.h>
#include <qlibrary.h>
#include <qtooltip.h>

#include "Qtstalker.h"
#include "Quote.h"
#include "DataWindow.h"
#include "EditDialog.h"
#include "ChartDb.h"
#include "ChartPage.h"
#include "GroupPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"

#include "grid.xpm"
#include "datawindow.xpm"
#include "indicator.xpm"
#include "quotes.xpm"
#include "done.xpm"
#include "configure.xpm"
#include "scaletoscreen.xpm"
#include "plugin.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "co.xpm"
#include "qtstalker.xpm"
#include "nav.xpm"
#include "dirclosed.xpm"
#include "plainitem.xpm"
#include "portfolio.xpm"
#include "test.xpm"
#include "text.xpm"
#include "buyarrow.xpm"
#include "sellarrow.xpm"
#include "fib.xpm"
#include "horizontal.xpm"
#include "vertical.xpm"
#include "trend.xpm"

QtstalkerApp::QtstalkerApp()
{
  config = 0;
  chartMenu = 0;
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

  navTab = new QTabWidget(navSplitter);

  initChartNav();
  initGroupNav();
  initPortfolioNav();
  initTestNav();

  // construct the chart areas

  QWidget *chartBase = new QWidget(navSplitter);

  QVBoxLayout *vbox = new QVBoxLayout(chartBase);

  split = new QSplitter(chartBase);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  mainPlot = new Plot (split);
  mainPlot->setDateFlag(TRUE);
  mainPlot->setMainFlag(TRUE);
  setPlotFont(mainPlot);
  QObject::connect(mainPlot, SIGNAL(rightMouseButton()), this, SLOT(mainPlotPopupMenu()));
  QObject::connect(mainPlot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(mainPlot, SIGNAL(chartObjectCreated(Setting *)), this, SLOT(slotChartObjectCreated(Setting *)));

  tabs = new QTabWidget(split);
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
  compressionChanged(config->getData(Config::Compression));

  // set the grid status
  s = config->getData(Config::Grid);
  if (s.toInt())
    actionGrid->setOn(TRUE);
  else
    actionGrid->setOn(FALSE);
  mainPlot->setGridFlag(s.toInt());

  // set the scale to screen status
  s = config->getData(Config::ScaleToScreen);
  if (s.toInt())
    actionScaleToScreen->setOn(TRUE);
  else
    actionScaleToScreen->setOn(FALSE);
  mainPlot->setScaleToScreen(s.toInt());

  // set the indicator splitter size
  QValueList<int> sizeList;
  s = config->getData(Config::MainPlotSize);
  sizeList.append(s.toInt());
  s = config->getData(Config::IndicatorPlotSize);
  sizeList.append(s.toInt());
  split->setSizes(sizeList);

  // set the nav splitter size
  sizeList.clear();
  s = config->getData(Config::NavAreaSize);
  sizeList.append(s.toInt());
  navSplitter->setSizes(sizeList);

  QStringList l = config->getIndicators();
  for (loop = 0; loop < (int) l.count(); loop++)
    addIndicatorButton(l[loop]);

  l = QStringList::split(" ", config->getData(Config::AppFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  qApp->setFont(font, TRUE, 0);
  
  // set the nav status
  slotHideNav(TRUE);

  slotChartTypeChanged(0);
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

  icon = plugin;
  actionNewPlugin = new QAction(tr("Install new plugins..."), icon, tr("Install new plugins..."), 0, this);
  actionNewPlugin->setStatusTip(tr("Install new plugins."));
  connect(actionNewPlugin, SIGNAL(activated()), this, SLOT(slotNewPlugin()));
  
  icon = nav;
  actionNav = new QAction(tr("Chart Navigator"), icon, tr("Chart Navigator"), 0, this, 0, true);
  actionNav->setStatusTip(tr("Toggle the chart navigator area."));
  connect(actionNav, SIGNAL(toggled(bool)), this, SLOT(slotHideNav(bool)));
}

void QtstalkerApp::initMenuBar()
{
  fileMenu = new QPopupMenu();
  fileMenu->insertSeparator();
  actionQuit->addTo(fileMenu);

  editMenu = new QPopupMenu();
  actionNewIndicator->addTo(editMenu);
  actionOptions->addTo(editMenu);

  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
  actionGrid->addTo(viewMenu);
  actionScaleToScreen->addTo(viewMenu);
  actionNav->addTo(viewMenu);

  pluginMenu = new QPopupMenu();
  actionNewPlugin->addTo(pluginMenu);

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
  chartMenu->insertItem(QPixmap(indicator), tr("New Indicator"), this, SLOT(slotNewIndicator()));
  chartDeleteMenu = new QPopupMenu();
  chartEditMenu = new QPopupMenu();
  chartMenu->insertItem(QPixmap(edit), tr("Edit Indicator"), chartEditMenu);
  chartMenu->insertItem (QPixmap(deletefile), tr("Delete Indicator"), chartDeleteMenu);
  chartMenu->insertSeparator ();
  chartObjectDeleteMenu = new QPopupMenu();
  chartObjectEditMenu = new QPopupMenu();

  chartObjectMenu = new QPopupMenu();
  QStringList l = mainPlot->getChartObjectList();
  int id = chartObjectMenu->insertItem(QPixmap(buyarrow), l[0], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(sellarrow), l[1], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(fib), l[2], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(horizontal), l[3], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(vertical), l[4], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(trend), l[5], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);
  id = chartObjectMenu->insertItem(QPixmap(text), l[6], this, SLOT(slotNewChartObject(int)));
  chartObjectMenu->setItemParameter(id, id);

  chartMenu->insertItem (QPixmap(co), tr("New Chart Object"), chartObjectMenu);

  chartMenu->insertItem (QPixmap(edit), tr("Edit Chart Object"), chartObjectEditMenu);
  chartMenu->insertItem (QPixmap(deletefile), tr("Delete Chart Object"), chartObjectDeleteMenu);
}

void QtstalkerApp::initToolBar()
{
  // construct the chart toolbar
  toolbar = new QToolBar(this, "toolbar");

  actionNav->addTo(toolbar);
  actionGrid->addTo(toolbar);
  actionScaleToScreen->addTo(toolbar);
  actionNewIndicator->addTo(toolbar);
  actionDatawindow->addTo(toolbar);
  actionQuotes->addTo(toolbar);

  compressionCombo = new QComboBox(toolbar);
  compressionCombo->show();
  QStringList l;
  l.append(tr("Daily"));
  l.append(tr("Weekly"));
  l.append(tr("Monthly"));
  compressionCombo->insertStringList(l, -1);
  QToolTip::add(compressionCombo, tr("Chart Compression"));
  connect(compressionCombo, SIGNAL(activated(int)), this, SLOT(slotCompressionChanged(int)));

  chartTypeCombo = new QComboBox(toolbar);
  chartTypeCombo->show();
  l.clear();
  l.append(tr("Bar"));
  l.append(tr("Paint Bar"));
  l.append(tr("Line"));
  l.append(tr("Candle"));
  l.append(tr("Point and Figure"));
  chartTypeCombo->insertStringList(l, -1);
  QToolTip::add(chartTypeCombo, tr("Chart Type"));
  connect(chartTypeCombo, SIGNAL(activated(int)), this, SLOT(slotChartTypeChanged(int)));

  pixelspace = new QSpinBox(toolbar);
  connect (pixelspace, SIGNAL(valueChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  QToolTip::add(pixelspace, tr("Bar Spacing"));

  barCombo = new QComboBox(toolbar);
  QToolTip::add(barCombo, tr("Bars"));

  toolbar->addSeparator();

  slider = new QSlider(toolbar);
  slider->setOrientation(Qt::Horizontal);
  connect (slider, SIGNAL(valueChanged(int)), this, SLOT(slotSliderChanged(int)));
  slider->setEnabled(FALSE);
  QToolTip::add(slider, tr("Pan Chart"));

  toolbar->setStretchableWidget(slider);
}

void QtstalkerApp::slotQuit()
{
  QValueList<int> list = split->sizes();
  config->setData(Config::MainPlotSize, QString::number(list[0]));
  config->setData(Config::IndicatorPlotSize, QString::number(list[1]));
  delete config;
  
  if (recordList)
    delete recordList;

  qApp->quit();
}

void QtstalkerApp::slotAbout()
{
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  							    tr("Qtstalker\nVersion 0.18 \n(C) 2001-2003 by Stefan Stratigakos"),
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
    config->setData(Config::PlotFont, set->getData(tr("Plot Font")));
    config->setData(Config::AppFont, set->getData(tr("App Font")));

    setPlotColor(mainPlot, Config::BackgroundColor);
    setPlotColor(mainPlot, Config::BorderColor);
    setPlotColor(mainPlot, Config::GridColor);
    setPlotColor(mainPlot, Config::UpColor);
    setPlotColor(mainPlot, Config::DownColor);
    setPlotColor(mainPlot, Config::NeutralColor);

    QDictIterator<Plot> it(plotList);
    for(; it.current(); ++it)
    {
      setPlotColor(it.current(), Config::BackgroundColor);
      setPlotColor(it.current(), Config::BorderColor);
      setPlotColor(it.current(), Config::GridColor);
      setPlotColor(it.current(), Config::UpColor);
      setPlotColor(it.current(), Config::DownColor);
      setPlotColor(it.current(), Config::NeutralColor);
    }

    QStringList l = QStringList::split(" ", config->getData(Config::AppFont), FALSE);
    QFont font(l[0], l[1].toInt(), l[2].toInt());
    qApp->setFont(font, TRUE, 0);

    setPlotFont(mainPlot);
    for(it.toFirst(); it.current(); ++it)
      setPlotFont(it.current());

    loadChart(chartPath);

    statusBar()->message (tr("Preferences saved."));
  }

  delete set;
  delete dialog;
}

void QtstalkerApp::slotGrid (bool state)
{
  config->setData(Config::Grid, QString::number(state));

  mainPlot->setGridFlag(state);
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setGridFlag(state);
}

void QtstalkerApp::slotScaleToScreen (bool state)
{
  config->setData(Config::ScaleToScreen, QString::number(state));

  mainPlot->setScaleToScreen(state);
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setScaleToScreen(state);
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
  if (db->openChart(chartPath))
  {
    qDebug(tr("Unable to open chart."));
    delete db;
    return;
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
  mainPlot->setInterval(pi);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setInterval(pi);

  mainPlot->setData(recordList);
  for(it.toFirst(); it.current(); ++it)
  {
    it.current()->setData(recordList);
    it.current()->setPixelspace(mainPlot->getPixelspace());
  }

  QStringList l = config->getIndicators();

  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Indicator *i = new Indicator;
    i->parse(config->getIndicator(l[loop]));
    
    QString s = config->getData(Config::IndicatorPluginPath);
    s.append("/");
    s.append(i->getData(QObject::tr("Type")));
    s.append(".so");

    QLibrary *lib = new QLibrary(s);
    Plugin *(*so)() = 0;
    so = (Plugin *(*)()) lib->resolve("create");
    if (so)
    {
      Plugin *plug = (*so)();

      plug->setIndicatorInput(recordList);

      plug->parse(i->getString());

      plug->calculate();

      int loop2;
      for (loop2 = 0; loop2 < plug->getIndicatorLines(); loop2++)
      {
        PlotLine *tpl = plug->getIndicatorLine(loop2);
        PlotLine *pl = new PlotLine;
        pl->setColor(tpl->getColor());
        pl->setType(tpl->getType());
        pl->setLabel(tpl->getLabel());

        int loop3;
        for (loop3 = 0; loop3 < (int) tpl->getSize(); loop3++)
          pl->append(tpl->getData(loop3));

        i->addLine(pl);
      }

      s = i->getData(QObject::tr("Alert"));
      if (! s.compare(QObject::tr("True")))
        i->setAlerts(plug->getAlerts());

      if (i->getMainPlot() && otherFlag)
        i->clearLines();

      if (i->getMainPlot())
        mainPlot->addIndicator(l[loop], i);
      else
      {
        Plot *plot = plotList[l[loop]];
        plot->addIndicator(l[loop], i);
      }

      delete plug;
    }

    delete lib;
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
  
  // set up the paint bar if needed
  QString s = config->getData(Config::ChartStyle);
  if (! s.compare(tr("Paint Bar")))
  {
    QString ind = config->getData(Config::PaintBarIndicator);
    Indicator *i = mainPlot->getIndicator(ind);
    if (! i)
    {
      Plot *plot = plotList[ind];
      i = plot->getIndicator(ind);
    }
      
    if (i)
      mainPlot->setAlerts(i->getAlerts());
  }

  int page = mainPlot->getWidth() / mainPlot->getPixelspace();
  int max = recordList->count() - page;
  if (max < 0)
    max = 0;
  slider->blockSignals(TRUE);
  slider->setRange(0, recordList->count() - 1);
  slider->setValue(max + 1);
  slider->blockSignals(FALSE);

  mainPlot->setIndex(max + 1);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setIndex(max + 1);

  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->blockSignals(FALSE);

  mainPlot->draw();
  
  if (plotList.count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }

  setCaption(getWindowCaption());

  delete db;
}

void QtstalkerApp::slotBarComboChanged (int index)
{
  config->setData(Config::Bars, barCombo->text(index));
  loadChart(chartPath);
}

void QtstalkerApp::slotNewChartObject (int id)
{
  QString selection = chartObjectMenu->text(id);

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

  if (chartObjectId)
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->createChartObject(selection, name);
  }
  else
    mainPlot->createChartObject(selection, name);
}

void QtstalkerApp::slotChartObjectCreated (Setting *co)
{
  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  db->setChartObject(co->getData("Name"), co);
  delete db;
}

void QtstalkerApp::slotEditChartObject (int id)
{
  QString name = chartObjectEditMenu->text(id);
  bool flag = FALSE;
  Indicator *i = mainPlot->getIndicator("Main Plot");
  Setting *co = i->getChartObject(name);
  if (! co)
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    i = plot->getIndicator(tabs->label(tabs->currentPageIndex()));
    co = i->getChartObject(name);
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
    db->openChart(chartPath);
    db->setChartObject(co->getData("Name"), co);
    delete db;

    if (! flag)
      mainPlot->draw();
    else
    {
      Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
      plot->draw();
    }
  }

  delete dialog;
}

void QtstalkerApp::slotDeleteChartObject (int id)
{
  QString name = chartObjectDeleteMenu->text(id);
  bool flag = FALSE;
  Indicator *i = mainPlot->getIndicator("Main Plot");
  Setting *co = i->getChartObject(name);
  if (! co)
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    i = plot->getIndicator(tabs->label(tabs->currentPageIndex()));
    co = i->getChartObject(name);
    flag = TRUE;
  }

  i->deleteChartObject(name);

  ChartDb *db = new ChartDb();
  db->openChart(chartPath);
  db->deleteChartObject(name);
  delete db;

  if (! flag)
    mainPlot->draw();
  else
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
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

void QtstalkerApp::plotPopupMenu (int area)
{
  chartEditMenu->clear();
  chartDeleteMenu->clear();

  int loop;
  QStringList l = config->getIndicators();
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    int id = chartDeleteMenu->insertItem(QPixmap(indicator), l[loop], this, SLOT(slotDeleteIndicator(int)));
    chartDeleteMenu->setItemParameter(id, id);

    id = chartEditMenu->insertItem(QPixmap(indicator), l[loop], this, SLOT(slotEditIndicator(int)));
    chartEditMenu->setItemParameter(id, id);
  }

  chartMenu->setItemParameter(chartObjectId, area);
}

void QtstalkerApp::mainPlotPopupMenu ()
{
  plotPopupMenu(0);
  
  chartObjectEditMenu->clear();
  chartObjectDeleteMenu->clear();

  Indicator *i = mainPlot->getIndicator("Main Plot");
  QStringList l = i->getChartObjects();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QPixmap icon;
    Setting *co = i->getChartObject(l[loop]);
    switch (co->getInt("ObjectType"))
    {
      case Plot::VerticalLine:
        icon = vertical;
        break;
      case Plot::HorizontalLine:
        icon = horizontal;
        break;
      case Plot::TrendLine:
        icon = trend;
        break;
      case Plot::Text:
        icon = text;
        break;
      case Plot::BuyArrow:
        icon = buyarrow;
        break;
      case Plot::SellArrow:
        icon = sellarrow;
        break;
      case Plot::FibonacciLine:
        icon = fib;
        break;
      default:
        break;
    }

    int id = chartObjectEditMenu->insertItem(icon, l[loop], this, SLOT(slotEditChartObject(int)));
    chartObjectEditMenu->setItemParameter(id, id);

    id = chartObjectDeleteMenu->insertItem(icon, l[loop], this, SLOT(slotDeleteChartObject(int)));
    chartObjectDeleteMenu->setItemParameter(id, id);
  }

  chartObjectId = 0;
  chartMenu->exec(QCursor::pos());
}

void QtstalkerApp::indicatorPlotPopupMenu ()
{
  if (status == None)
    return;

  plotPopupMenu(1);

  chartObjectEditMenu->clear();
  chartObjectDeleteMenu->clear();

  Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
  Indicator *i = plot->getIndicator(tabs->label(tabs->currentPageIndex()));
  QStringList l = i->getChartObjects();
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    QPixmap icon;
    Setting *co = i->getChartObject(l[loop]);
    switch (co->getInt("ObjectType"))
    {
      case Plot::VerticalLine:
        icon = vertical;
        break;
      case Plot::HorizontalLine:
        icon = horizontal;
        break;
      case Plot::TrendLine:
        icon = trend;
        break;
      case Plot::Text:
        icon = text;
        break;
      case Plot::BuyArrow:
        icon = buyarrow;
        break;
      case Plot::SellArrow:
        icon = sellarrow;
        break;
      case Plot::FibonacciLine:
        icon = fib;
        break;
      default:
        break;
    }

    int id = chartObjectEditMenu->insertItem(icon, l[loop], this, SLOT(slotEditChartObject(int)));
    chartObjectEditMenu->setItemParameter(id, id);

    id = chartObjectDeleteMenu->insertItem(icon, l[loop], this, SLOT(slotDeleteChartObject(int)));
    chartObjectDeleteMenu->setItemParameter(id, id);
  }

  chartObjectId = 1;
  chartMenu->exec(QCursor::pos());
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

  mainPlot->setInterval(t);

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setInterval(t);
}

void QtstalkerApp::slotChartTypeChanged (int)
{
  config->setData(Config::ChartStyle, chartTypeCombo->currentText());
  mainPlot->setChartType(chartTypeCombo->currentText());

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setPixelspace(mainPlot->getPixelspace());

  QString s = config->getData(Config::ChartStyle);
  if (! s.compare(tr("Paint Bar")))
  {
    QString ind = config->getData(Config::PaintBarIndicator);
    if (ind.length())
    {
      Indicator *i = mainPlot->getIndicator(ind);
      if (! i)
      {
        Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
        i = plot->getIndicator(ind);
      }

      if (i)
        mainPlot->setAlerts(i->getAlerts());
    }
  }

  pixelspace->blockSignals(TRUE);
  pixelspace->setRange(mainPlot->getMinPixelspace(), 99);
  pixelspace->blockSignals(FALSE);

  mainPlot->draw();
  
  if (plotList.count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }
}

void QtstalkerApp::slotNewIndicator ()
{
  bool ok;
  QStringList list = config->getIndicatorPlugins();
  QString selection = QInputDialog::getItem(tr("New Indicator"),
  							   tr("Select an indicator create."),
							   list,
							   0,
							   FALSE,
							   &ok,
							   this);
  if (! ok || ! selection.length())
    return;

  QString name = QInputDialog::getText(tr("Indicator Name"),
  						      tr("Enter a unique name for this indicator."),
						      QLineEdit::Normal,
						      selection,
						      &ok,
						      this);
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

  QString s = config->getData(Config::IndicatorPluginPath);
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
    {
      set->set(key[loop], plug->getData(key[loop]), plug->getType(key[loop]));
      set->setList(key[loop], plug->getList(key[loop]));
    }

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
  
  QString s = config->getData(Config::IndicatorPluginPath);
  s.append("/");
  s.append(set->getData("Type"));
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
    {
      if (plug->getType(key[loop]) == Setting::List)
        set->setList(key[loop], plug->getList(key[loop]));
    }

    delete plug;
  }

  delete lib;

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

  Indicator *i = mainPlot->getIndicator(text);
  if (! i)
  {
    if (tabs->count() == 1)
    {
      QMessageBox::information(this, tr("Qtstalker: Error"), tr("Must leave at least one indicator tab."));
      return;
    }

    Plot *plot = plotList[text];
    i = plot->getIndicator(text);

    tabs->removePage((QWidget *) plot);

    // delete any chart objects that belong to the indicator
    QStringList l = i->getChartObjects();

    ChartDb *db = new ChartDb;
    db->openChart(chartPath);

    int loop;
    for (loop = 0; loop < (int) l.count(); loop++)
      db->deleteChartObject(l[loop]);

    delete db;
  }

  config->deleteIndicator(text);

  loadChart(chartPath);
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  mainPlot->setPixelspace(d);
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setPixelspace(d);

  if (plotList.count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }
}

void QtstalkerApp::slotSliderChanged (int v)
{
  if (status == None)
    return;

  mainPlot->setIndex(v);
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->setIndex(v);

  if (plotList.count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }
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

  Plot *plot = new Plot (baseWidget);
  QObject::connect(plot, SIGNAL(rightMouseButton()), this, SLOT(indicatorPlotPopupMenu()));
  QObject::connect(plot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(plot, SIGNAL(chartObjectCreated(Setting *)), this, SLOT(slotChartObjectCreated(Setting *)));
  plotList.replace(d, plot);
  plot->setGridFlag(config->getData(Config::Grid).toInt());
  plot->setScaleToScreen(config->getData(Config::ScaleToScreen).toInt());

  setPlotColor(plot, Config::BackgroundColor);
  setPlotColor(plot, Config::BorderColor);
  setPlotColor(plot, Config::GridColor);
  setPlotColor(plot, Config::UpColor);
  setPlotColor(plot, Config::DownColor);
  setPlotColor(plot, Config::NeutralColor);

  setPlotFont(plot);

  tabs->addTab(plot, d);

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

void QtstalkerApp::slotChartUpdated ()
{
  loadChart(chartPath);
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

void QtstalkerApp::setPlotFont (Plot *plot)
{
  QStringList l = QStringList::split(" ", config->getData(Config::PlotFont), FALSE);
  QFont font(l[0], l[1].toInt(), l[2].toInt());
  plot->setPlotFont(font);
}

void QtstalkerApp::setPlotColor (Plot *plot, Config::Parm parm)
{
  QColor color;
  color.setNamedColor(config->getData(parm));

  switch(parm)
  {
    case Config::BackgroundColor:
      plot->setBackgroundColor(color);
      break;
    case Config::BorderColor:
      plot->setBorderColor(color);
      break;
    case Config::GridColor:
      plot->setGridColor(color);
      break;
    case Config::UpColor:
      plot->setUpColor(color);
      break;
    case Config::DownColor:
      plot->setDownColor(color);
      break;
    case Config::NeutralColor:
      plot->setNeutralColor(color);
      break;
    default:
      break;
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
  ChartPage *cp = new ChartPage(baseWidget, config);
  connect(cp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  navTab->addTab(cp, QIconSet(QPixmap(plainitem)), QString::null);
  navTab->setTabToolTip(cp, tr("Workwith Charts"));
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

void QtstalkerApp::slotHideNav (bool d)
{
  actionNav->setOn(d);

  if (d)
    navTab->show();
  else
    navTab->hide();
}

/*
void QtstalkerApp::initInfoNav ()
{
  QWidget *w = new QWidget(baseWidget);

  QVBoxLayout *vbox = new QVBoxLayout(w);

  infoLabel = new QLabel(w);
  vbox->addWidget(infoLabel);

  QString s = currentDir.absPath();
  s.append("/");
  s.append(list->currentText());

  ChartDb *db = new ChartDb;
  if (db->openChart(s))
  {
    delete db;
    clearFileInfo();
    return;
  }

  Setting *set = db->getDetails();

  symbol->setText(set->getData("Symbol"));

  title->setText(set->getData("Title"));

  type->setText(set->getData("Chart Type"));

  s = set->getData("First Date");
  s.truncate(s.length() - 6);
  firstDate->setText(s);

  s = set->getData("Last Date");
  s.truncate(s.length() - 6);
  lastDate->setText(s);

  delete db;

  navTab->addTab(w, "I");
}
*/

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





