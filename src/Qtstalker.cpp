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
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "HelpWindow.h"

#include "indicator.xpm"
#include "qtstalker.xpm"
#include "dirclosed.xpm"
#include "plainitem.xpm"
#include "portfolio.xpm"
#include "test.xpm"
#include "scanner.xpm"
#include "macro.xpm"

QtstalkerApp::QtstalkerApp()
{
  recordList = 0;
  quoteDialog = 0;
  status = None;
  plotList.setAutoDelete(TRUE);
  setIcon(qtstalker);

  config.setup();

  initMenuBar();

  baseWidget = new QWidget(this);
  setCentralWidget (baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout(baseWidget);

  navSplitter = new QSplitter(baseWidget);
  navSplitter->setOrientation(Horizontal);
  hbox->addWidget(navSplitter);
  
  navBase = new QWidget(navSplitter);
  QVBoxLayout *vbox = new QVBoxLayout(navBase);
  
  QHBoxLayout *hbox2 = new QHBoxLayout(vbox);
  hbox2->setSpacing(1);
  hbox2->setMargin(2);
  
  bg = new QButtonGroup(navBase);
  QObject::connect(bg, SIGNAL(clicked(int)), this, SLOT(slotNavigatorButtonPressed(int)));
  bg->setExclusive(TRUE);
  bg->hide();
  
  QPushButton *button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Charts"));
  button->setPixmap(plainitem);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 0);
  bg->setButton(0);
  
  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Groups"));
  button->setPixmap(dirclosed);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 1);

  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Main Chart Indicators"));
  button->setPixmap(indicator);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 2);
  
  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Portfolios"));
  button->setPixmap(portfolio);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 3);

  hbox2->addStretch(1);
    
  hbox2 = new QHBoxLayout(vbox);
  hbox2->setSpacing(1);
  hbox2->setMargin(2);

  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Backtesting"));
  button->setPixmap(test);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 4);
  
  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Scanner"));
  button->setPixmap(scanner);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 5);
  
  button = new QPushButton(navBase);
  QToolTip::add(button, tr("Workwith Macro"));
  button->setPixmap(macro);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox2->addWidget(button);
  bg->insert(button, 6);
  
  hbox2->addStretch(1);
  
  // setup the data panel splitter
  
  dpSplitter = new QSplitter(navBase);
  dpSplitter->setOrientation(Vertical);
  vbox->addWidget(dpSplitter);
  
  navTab = new NavigatorTab(dpSplitter);
  QObject::connect(navTab, SIGNAL(signalPositionChanged(int)), this, SLOT(slotNavigatorPosition(int)));
  
  infoLabel = new QMultiLineEdit(dpSplitter);
  infoLabel->setReadOnly(TRUE);

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
  color.setNamedColor(config.getData(Config::BackgroundColor));
  QObject::connect(this, SIGNAL(signalBackgroundColor(QColor)), mainPlot, SLOT(setBackgroundColor(QColor)));
  emit signalBackgroundColor(color);

  color.setNamedColor(config.getData(Config::BorderColor));
  QObject::connect(this, SIGNAL(signalBorderColor(QColor)), mainPlot, SLOT(setBorderColor(QColor)));
  emit signalBorderColor(color);

  color.setNamedColor(config.getData(Config::GridColor));
  QObject::connect(this, SIGNAL(signalGridColor(QColor)), mainPlot, SLOT(setGridColor(QColor)));
  emit signalGridColor(color);

  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), mainPlot, SLOT(setPlotFont(QFont)));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  if (l.count() == 3)
  {
    QFont font(l[0], l[1].toInt(), l[2].toInt());
    emit signalPlotFont(font);
  }
  else
    emit signalPlotFont(QFont());

  QObject::connect(mainPlot, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  QObject::connect(mainPlot, SIGNAL(signalEditIndicator(QString)), this, SLOT(slotEditIndicator(QString)));

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
  QObject::connect(this, SIGNAL(signalInterval(BarData::BarCompression)), mainPlot, SLOT(setInterval(BarData::BarCompression)));
  QObject::connect(this, SIGNAL(signalChartPath(QString)), mainPlot, SLOT(setChartPath(QString)));
  QObject::connect(this, SIGNAL(signalDrawMode(bool)), mainPlot, SLOT(setDrawMode(bool)));

  tabs = new IndicatorTab(split);
  QObject::connect(tabs, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged(QWidget *)));

  initToolBar();

  // setup the compression list
  compressionChanged();

  // set the grid status
  emit signalGrid(menubar->getStatus("grid"));

  // set the scale to screen status
  emit signalScaleToScreen(menubar->getStatus("scaleToScreen"));

  // set the logscale status
  mainPlot->setLogScale(menubar->getStatus("log"));
  
  // set the nav splitter size
  QString s = config.getData(Config::NavAreaSize);
  QValueList<int> sizeList;
  sizeList.append(s.toInt());
  QString s2 = config.getData(Config::Width);
  sizeList.append(s2.toInt() - s.toInt());
  navSplitter->setSizes(sizeList);
  
  // place navigator on left/right side
  if (navTab->getPosition() == 0)
    navSplitter->moveToLast(navBase);
    
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initPortfolioNav();
  initTestNav();
  initScannerNav();
  initMacroNav();

  l = config.getIndicators();
  QStringList l2 = QStringList::split(",", config.getData(Config::IndicatorPageStatus), FALSE);
  int loop;
  for (loop = 0; loop < (int) l2.count(); loop++)
    l.remove(l2[loop]);
  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = config.getIndicator(l[loop]);
    s = set->getData("plotType");
    if (! s.length())
      addIndicatorButton(l[loop], Indicator::TabPlot);
    else
      addIndicatorButton(l[loop], (Indicator::PlotType) s.toInt());
      
    Indicator *i = new Indicator;    
    i->setName(l[loop]);
    QString s2 = config.getData(Config::IndicatorPath) + "/" + l[loop];
    i->setFile(s2);
    i->setType(set->getData("plugin"));
    if (s.length())
      i->setPlotType((Indicator::PlotType) s.toInt());
    loadIndicator(i);
      
    delete set;
  }

  l = QStringList::split(",", config.getData(Config::AppFont), FALSE);
  if (l.count() == 3)
  {
    QFont font2(l[0], l[1].toInt(), l[2].toInt());
    qApp->setFont(font2, TRUE, 0);
  }
  else
    qApp->setFont(QFont(), TRUE, 0);
  
  // set the indicator splitter sizes
  sizeList = split->sizes();
  l2 = QStringList::split(",", config.getData(Config::PlotSizes), FALSE);
  for (loop = 0; loop < (int) l2.count(); loop++)
    sizeList[loop] = l2[loop].toInt();
  split->setSizes(sizeList);
  
  // set the data panel splitter size
  sizeList = dpSplitter->sizes();
  l = QStringList::split(",", config.getData(Config::DataPanelSize), FALSE);
  for (loop = 0; loop < (int) l.count(); loop++)
    sizeList[loop] = l[loop].toInt();
  dpSplitter->setSizes(sizeList);

  
  // set the nav status
  slotHideNav(TRUE);

  // set the last used chart type
  slotChartTypeChanged(0);

  resize(config.getData(Config::Width).toInt(), config.getData(Config::Height).toInt());
  
  // set crosshairs status for all the plots
  emit signalCrosshairsStatus(config.getData(Config::Crosshairs).toInt());  
  
  // set the drawmode status for all the plots
  emit signalDrawMode(menubar->getStatus("drawMode"));
  
  navTab->togglePosition(navTab->getPosition());

  // setup the indicator page  
  ip->refreshList();
  
  // make sure the focus is set to the chart panel
  slotNavigatorButtonPressed (0);
  
  // catch any kill signals and try to save config
  QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));
  
  statusBar()->message(tr("Ready"), 2000);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initMenuBar()
{
  menubar = new MainMenubar(this);
  connect(menubar, SIGNAL(signalExit()), qApp, SLOT(quit()));
}

void QtstalkerApp::initToolBar()
{
  // construct the chart toolbar
  toolbar = new QToolBar(this, "buttonToolbar");
  menubar->getAction("exit")->addTo(toolbar);
  menubar->getAction("options")->addTo(toolbar);
  menubar->getAction("sidePanel")->addTo(toolbar);
  menubar->getAction("grid")->addTo(toolbar);
  menubar->getAction("scaleToScreen")->addTo(toolbar);
  menubar->getAction("log")->addTo(toolbar);
  menubar->getAction("hideMain")->addTo(toolbar);
  menubar->getAction("indicatorDate")->addTo(toolbar);
  menubar->getAction("drawMode")->addTo(toolbar);
  menubar->getAction("newIndicator")->addTo(toolbar);
  menubar->getAction("dataWindow")->addTo(toolbar);
  menubar->getAction("quotes")->addTo(toolbar);
  menubar->getAction("help")->addTo(toolbar);

  toolbar2 = new ChartToolbar(this);//new
  connect(toolbar2, SIGNAL(signalCompressionChanged(int)), this, SLOT(slotCompressionChanged(int)));
  connect(toolbar2, SIGNAL(signalChartTypeChanged(int)), this, SLOT(slotChartTypeChanged(int)));
  connect(toolbar2, SIGNAL(signalPixelspaceChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  connect(toolbar2, SIGNAL(signalSliderChanged(int)), this, SLOT(slotSliderChanged(int)));
}

void QtstalkerApp::slotQuit()
{
  // save indicatorPage status
  ip->saveStatus();
  
  // save any chart data
  mainPlot->clear();
  
  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();

  // save plot sizes
  QValueList<int> list = split->sizes();
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) list.count(); loop++)
    l.append(QString::number(list[loop]));
  config.setData(Config::PlotSizes, l.join(","));
  
  // save data panel size
  list = dpSplitter->sizes();
  l.clear();
  for (loop = 0; loop < (int) list.count(); loop++)
    l.append(QString::number(list[loop]));
  config.setData(Config::DataPanelSize, l.join(","));
  
  config.setData(Config::Height, QString::number(this->height()));
  config.setData(Config::Width, QString::number(this->width()));
  config.setData(Config::X, QString::number(this->x()));
  config.setData(Config::Y, QString::number(this->y()));
  config.setData(Config::Crosshairs, QString::number(mainPlot->getCrosshairsStatus()));
  config.closePlugins();
  
  menubar->saveSettings();
  toolbar2->saveSettings();

  // delete any BarData
  if (recordList)
    delete recordList;
    
  if (quoteDialog)
    delete quoteDialog;
}

void QtstalkerApp::slotAbout()
{
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					tr("Qtstalker\nVer CVS 0.30 (greased weasel)\n(C) 2001-2005 by Stefan Stratigakos"),
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
  toolbar2->enableSlider(TRUE);//new
  status = Chart;
  qApp->processEvents();
  loadChart(selection);
}

void QtstalkerApp::slotQuotes ()
{
  if (quoteDialog)
    quoteDialog->raise();
  else
  {
    quoteDialog = new QuoteDialog();
    QObject::connect(quoteDialog, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
    QObject::connect(quoteDialog, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
    QObject::connect(quoteDialog, SIGNAL(destroyed()), this, SLOT(slotExitQuoteDialog()));
    quoteDialog->show();
  }
}

void QtstalkerApp::slotOptions ()
{
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Edit Prefs"));
  dialog->setHelpFile("preferences.html");

  dialog->createPage(tr("General"));
  dialog->addCheckItem(tr("Main Menubar"), tr("General"), config.getData(Config::Menubar).toInt());
  
  dialog->createPage(tr("Colors"));
  dialog->addColorItem(tr("Chart Background"), tr("Colors"), QColor(config.getData(Config::BackgroundColor)));
  dialog->addColorItem(tr("Chart Border"), tr("Colors"), QColor(config.getData(Config::BorderColor)));
  dialog->addColorItem(tr("Chart Grid"), tr("Colors"), QColor(config.getData(Config::GridColor)));

  dialog->createPage(tr("Fonts"));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  if (l.count() == 3)
    dialog->addFontItem(tr("Plot Font"), tr("Fonts"), QFont(l[0], l[1].toInt(), l[2].toInt()));
  l = QStringList::split(",", config.getData(Config::AppFont), FALSE);
  if (l.count() == 3)
    dialog->addFontItem(tr("App Font"), tr("Fonts"), QFont(l[0], l[1].toInt(), l[2].toInt()));
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    // save the main menubar setting
    bool flag = dialog->getCheck(tr("Main Menubar"));
    config.setData(Config::Menubar, QString::number(flag));
    if (flag)
      menubar->show();
    else
      menubar->hide();
  
    QColor color = dialog->getColor(tr("Chart Background"));
    config.setData(Config::BackgroundColor, color.name());
    emit signalBackgroundColor(color);

    color = dialog->getColor(tr("Chart Border"));
    config.setData(Config::BorderColor, color.name());
    emit signalBorderColor(color);

    color = dialog->getColor(tr("Chart Grid"));
    config.setData(Config::GridColor, color.name());
    emit signalGridColor(color);

    // save plot font option
    QFont font = dialog->getFont(tr("Plot Font"));
    QString s = font.family();
    s.append(",");
    s.append(QString::number(font.pointSize()));
    s.append(",");
    s.append(QString::number(font.weight()));
    QString s2 = config.getData(Config::PlotFont);
    if (s.compare(s2))
    {
      config.setData(Config::PlotFont, s);
      emit signalPlotFont(font);
    }

    // save app font option
    font = dialog->getFont(tr("App Font"));
    s = font.family();
    s.append(",");
    s.append(QString::number(font.pointSize()));
    s.append(",");
    s.append(QString::number(font.weight()));
    s2 = config.getData(Config::AppFont);
    if (s.compare(s2))
    {
      config.setData(Config::AppFont, s);
      qApp->setFont(font, TRUE, 0);
    }
    
    loadChart(chartPath);

    statusBar()->message (tr("Preferences saved."));
  }

  delete dialog;
}

void QtstalkerApp::slotGrid (bool state)
{
  config.setData(Config::Grid, QString::number(state));
  
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
  config.setData(Config::ScaleToScreen, QString::number(state));

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
  config.setData(Config::LogScale, QString::number(state));
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
  QStringList di = ip->getDisabledIndicators();

  mainPlot->clear();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();
    
  // make sure we change db's after Plot saves previous db edits
  emit signalChartPath(chartPath);
  
  ChartDb *plug = new ChartDb;
  if (plug->openChart(chartPath))
  {
    qDebug("QtstalkerApp::loadChart: can't open db");
    delete plug;
    return;
  }
  plug->setBarCompression((BarData::BarCompression) toolbar2->getCompressionInt());
  plug->setBarRange(toolbar2->getBars());
  
  if (recordList)
    delete recordList;
  BarData *trl = plug->getHistory();
  recordList = new BarData;
  trl->copy(recordList);
  delete trl;
  
  chartName = plug->getTitle();
  chartType = plug->getType();
  chartSymbol = plug->getSymbol();
  
  mainPlot->setData(recordList);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);
    
  mainPlot->setChartInput ();

  QStringList l = config.getIndicators();
  int loop;
  for (loop = 0; loop < (int) di.count(); loop++)
    l.remove(di[loop]);

  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting *set = config.getIndicator(l[loop]);
    if (! set->count())
    {
      delete set;
      continue;
    }
    
    Indicator *i = new Indicator;
    i->setName(l[loop]);
    i->setFile(config.getData(Config::IndicatorPath) + "/" + l[loop]);
    i->setType(set->getData("plugin"));
    if (set->getData("plotType").length())
      i->setPlotType((Indicator::PlotType) set->getData("plotType").toInt());
    delete set;
    loadIndicator(i);
  }

  QPtrList<Setting> col = plug->getChartObjects();
  QPtrListIterator<Setting> coit(col);
  for (; coit.current(); ++coit)
  {
    Setting *co = coit.current();
    
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
  delete plug;

  // update the pixelspace
  toolbar2->setMinPixelspace(mainPlot->getMinPixelspace());
  
  setSliderStart(toolbar2->getPixelspace(), TRUE);
  
  mainPlot->draw();

  for(it.toFirst(); it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  slotTabChanged(0);

  setCaption(getWindowCaption());
}

void QtstalkerApp::loadIndicator (Indicator *i)
{
  IndicatorPlugin *plug = config.getIndicatorPlugin(i->getType());
  if (plug)
  {
    if (recordList)
      plug->setIndicatorInput(recordList);
    plug->loadIndicatorSettings(i->getFile());
    if (recordList)
      plug->calculate();
    i->copy(plug->getIndicator());

    if (i->getPlotType() == Indicator::MainPlot)
      mainPlot->addIndicator(i->getName(), i);
    else
    {
      Plot *plot = plotList[i->getName()];
      plot->addIndicator(i->getName(), i);
      if (recordList)
        plot->setData(recordList);
    }
  }
  
  config.closePlugin(i->getType());
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

  caption.append(toolbar2->getCompression());

  return caption;
}

void QtstalkerApp::slotDataWindow ()
{
  if (! recordList)
  {
    DataWindow *dw = new DataWindow(0, 0);
    dw->setCaption(getWindowCaption());
    dw->show();
    return;
  }
  
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
      dw->setData(loop, 2, mainPlot->strip(recordList->getOpen(loop), 4));
      dw->setData(loop, 3, mainPlot->strip(recordList->getHigh(loop), 4));
      dw->setData(loop, 4, mainPlot->strip(recordList->getLow(loop), 4));
      dw->setData(loop, 5, mainPlot->strip(recordList->getClose(loop), 4));
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
        dw->setData(loop3 + offset, col, mainPlot->strip(line->getData(loop3), 4));
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
          dw->setData(loop3 + offset, col, mainPlot->strip(line->getData(loop3), 4));
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
  config.setData(Config::Compression, QString::number(toolbar2->getCompressionInt()));
  emit signalInterval((BarData::BarCompression) toolbar2->getCompressionInt());
}

void QtstalkerApp::slotChartTypeChanged (int)
{
  if (mainPlot->setChartType(toolbar2->getChartType()))
    return;

  config.setData(Config::ChartStyle, toolbar2->getChartType());

  emit signalPixelspace(mainPlot->getPixelspace());

  toolbar2->setPixelspace(mainPlot->getMinPixelspace(), mainPlot->getPixelspace());
  
  setSliderStart(toolbar2->getPixelspace(), FALSE);

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
  Indicator *ti = new Indicator;
  QStringList l = ti->getPlotTypes();
  delete ti;
  
  PrefDialog *idialog = new PrefDialog;
  idialog->setCaption(tr("New Indicator"));
  idialog->createPage (tr("Details"));
  idialog->setHelpFile("newindicator.html");
  idialog->addComboItem(tr("Indicator"), tr("Details"), config.getIndicatorList(), 0);
  idialog->addTextItem(tr("Name"), tr("Details"), tr("NewIndicator"));
  idialog->addComboItem(tr("Plot Type"), tr("Details"), l, 1);
  
  int rc = idialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete idialog;
    return;
  }
  
  QString name = idialog->getText(tr("Name"));
  QString indicator = idialog->getCombo(tr("Indicator"));
  Indicator::PlotType plotType = (Indicator::PlotType) idialog->getComboIndex(tr("Plot Type"));
  delete idialog;
  
  if (! name.length())
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Indicator name missing."));
    return;
  }
  
  int loop;
  QString s;
  for (loop = 0; loop < (int) name.length(); loop++)
  {
    QChar c = name.at(loop);
    if (c.isLetterOrNumber())
      s.append(c);
  }
  name = s;

  QDir dir;
  if (dir.exists(config.getData(Config::IndicatorPath) + "/" + name))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("Duplicate indicator name."));
    return;
  }

  IndicatorPlugin *plug = config.getIndicatorPlugin(indicator);
  if (! plug)
  {
    qDebug("QtstalkerApp::slotNewIndicator - could not open plugin");
    config.closePlugin(indicator);
    return;
  }

  rc = plug->indicatorPrefDialog(this);
  
  if (rc)
  {
    plug->setPlotType((int) plotType);
    
    QString s = config.getData(Config::IndicatorPath) + "/" + name;
    plug->saveIndicatorSettings(s);
    
    addIndicatorButton(name, plotType);
    
    Indicator *i = new Indicator;
    i->setFile(s);
    i->setName(name);
    i->setType(indicator);
    i->setPlotType(plotType);
    loadIndicator(i);

    if (i->getPlotType() == Indicator::MainPlot)
      mainPlot->draw();

    emit signalIndicatorPageRefresh();
  }

  config.closePlugin(indicator);
}

void QtstalkerApp::slotEditIndicator (QString selection)
{
  Setting *set = config.getIndicator(selection);
  if (! set->count())
  {
    delete set;
    qDebug("QtstalkerApp::slotEditIndicator:indicator settings empty");
  }
  
  QString type = set->getData("plugin");
  
  IndicatorPlugin *plug = config.getIndicatorPlugin(type);
  if (! plug)
  {
    qDebug("QtstalkerApp::slotEditIndicator - could not open plugin");
    config.closePlugin(type);
    delete set;
    return;
  }

  QString s = config.getData(Config::IndicatorPath) + "/" + selection;
  plug->loadIndicatorSettings(s);
  int rc = plug->indicatorPrefDialog(this);

  if (rc)
  {
    plug->saveIndicatorSettings(s);

    Indicator *i = new Indicator;    
    i->setName(selection);
    i->setFile(s);
    i->setType(type);
    if (set->getData("plotType").length())
      i->setPlotType((Indicator::PlotType) set->getData("plotType").toInt());
    loadIndicator(i);
    
    if (i->getPlotType() == Indicator::MainPlot)
      mainPlot->draw();
    else
      slotTabChanged(0);
  }

  config.closePlugin(type);
  delete set;
}

void QtstalkerApp::slotDeleteIndicator (QString text)
{
  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        tr("Are you sure you want to permanently delete this indicator?"),
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);
  if (rc == QMessageBox::No)
    return;

  Setting *set = config.getIndicator(text);
  if (! set->count())
  {
    delete set;
    qDebug("QtstalkerApp::slotDeleteIndicator:indicator settings empty");
  }

  // delete any chart objects that belong to the indicator
  bool mainFlag = FALSE;
  bool tabFlag = FALSE;
  switch (set->getInt("plotType"))
  {
    case Indicator::MainPlot:
      mainFlag = TRUE;
      break;
    case Indicator::TabPlot:
      tabFlag = TRUE;
      break;
    default:
      break;      
  }
  
  if (! mainFlag)
  {
    ChartDb *db = new ChartDb;
    db->openChart(chartPath);
    QPtrList<Setting> l = db->getChartObjects ();
    QPtrListIterator<Setting> it(l);
    for (; it.current(); ++it)
    {
      Setting *co = it.current();
      if (! co->getData("Plot").compare(text))
        db->deleteChartObject(co->getData("Name"));
    }
    delete db;
  }

  if (! mainFlag)
  {
    if (tabFlag)
      tabs->removePage((QWidget *) plotList[text]);
    plotList.remove(text);
  }
  else
    mainPlot->deleteIndicator(text);

  config.deleteIndicator(text);

  emit signalIndicatorPageRefresh();

  if (mainFlag)
    mainPlot->draw();
}

void QtstalkerApp::slotDisableIndicator (QString name)
{
  QStringList l = mainPlot->getIndicators();
  if (l.findIndex(name) != -1)
  {
    mainPlot->deleteIndicator(name);
    mainPlot->draw();
  }
  else
  {
    QDictIterator<Plot> it(plotList);
    for(; it.current(); ++it)
    {
      l = it.current()->getIndicators();
      if (l.findIndex(name) != -1)
      {
        if (it.current()->getTabFlag())
	{
          tabs->removePage((QWidget *) it.current());
          plotList.remove(name);
	}
	else
          plotList.remove(name);
	  
	break;
      }
    }
  }
}

void QtstalkerApp::slotEnableIndicator (QString name)
{
  Config config;
  Setting *set = config.getIndicator(name);
  if (! set->count())
  {
    delete set;
    return;
  }
    
  addIndicatorButton(name, (Indicator::PlotType) set->getData("plotType").toInt());
  
  Indicator *i = new Indicator;
  i->setName(name);
  i->setFile(config.getData(Config::IndicatorPath) + "/" + name);
  i->setType(set->getData("plugin"));
  if (set->getData("plotType").length())
    i->setPlotType((Indicator::PlotType) set->getData("plotType").toInt());
  delete set;
  loadIndicator(i);
  
  if (i->getPlotType() == Indicator::MainPlot)
    mainPlot->draw();
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  int ov = mainPlot->getPixelspace();
  
  emit signalPixelspace(d);
  
  setSliderStart(ov, FALSE);
  
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
  toolbar2->setPixelspace(d, d);
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

void QtstalkerApp::addIndicatorButton (QString d, Indicator::PlotType tabFlag)
{
  Setting *set = config.getIndicator(d);
  if (! set->count())
  {
    delete set;
    qDebug("QtstalkerApp::addIndicatorButton:indicator settings empty");
  }
  
  QString type = set->getData("plugin");
  delete set;

  if (tabFlag == Indicator::MainPlot)
    return;

  Plot *plot;
  if (tabFlag == Indicator::TabPlot)
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
  QObject::connect(plot, SIGNAL(signalEditIndicator(QString)), this, SLOT(slotEditIndicator(QString)));
  QObject::connect(plot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  QObject::connect(plot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  
  QObject::connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));
  QObject::connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  plot->setIndex(toolbar2->getSlider());

  QObject::connect(this, SIGNAL(signalDrawMode(bool)), plot, SLOT(setDrawMode(bool)));

  QObject::connect(this, SIGNAL(signalInterval(BarData::BarCompression)), plot, SLOT(setInterval(BarData::BarCompression)));
  compressionChanged();

  QObject::connect(this, SIGNAL(signalGrid(bool)), plot, SLOT(setGridFlag(bool)));
  emit signalGrid(config.getData(Config::Grid).toInt());

  QObject::connect(this, SIGNAL(signalScaleToScreen(bool)), plot, SLOT(setScaleToScreen(bool)));
  emit signalScaleToScreen(config.getData(Config::ScaleToScreen).toInt());

  QObject::connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  emit signalPixelspace(toolbar2->getPixelspace());

  QColor color;
  color.setNamedColor(config.getData(Config::BackgroundColor));
  QObject::connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  emit signalBackgroundColor(color);

  color.setNamedColor(config.getData(Config::BorderColor));
  QObject::connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  emit signalBorderColor(color);

  color.setNamedColor(config.getData(Config::GridColor));
  QObject::connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  emit signalGridColor(color);

  QObject::connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  if (l.count() == 3)
  {
    QFont font(l[0], l[1].toInt(), l[2].toInt());
    emit signalPlotFont(font);
  }
  else
    emit signalPlotFont(QFont());

  plot->setDateFlag(menubar->getStatus("indicatorDate"));
  QObject::connect(this, SIGNAL(signalPlotDate(bool)), plot, SLOT(setDateFlag(bool)));

  // setup the crosshair signals
  QObject::connect(plot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  QObject::connect(plot, SIGNAL(signalCrosshairsStatus(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  QObject::connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot, SLOT(setCrosshairsStatus(bool)));

  // setup plot key presses
  QObject::connect(plot, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(slotPlotKeyPressed(QKeyEvent *)));

  if (tabFlag == Indicator::TabPlot)
  {
    tabs->addTab(plot, d);
    tabs->showPage(plot);
    tabs->adjustSize();
  }
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->refreshList();
  
  if (status == None)
    return;
  
  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusBar()->message(d);
  qApp->processEvents();
}

void QtstalkerApp::slotTabChanged (QWidget *)
{
  if (tabs->count())
  {
    Plot *plot = plotList[tabs->label(tabs->currentPageIndex())];
    plot->draw();
  }
}

void QtstalkerApp::initGroupNav ()
{
  gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  navTab->addWidget(gp, 1);
}

void QtstalkerApp::initChartNav ()
{
  chartNav = new ChartPage(baseWidget);
  connect(chartNav, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  navTab->addWidget(chartNav, 0);
}

void QtstalkerApp::initPortfolioNav ()
{
  pp = new PortfolioPage(baseWidget);
  navTab->addWidget(pp, 3);
}

void QtstalkerApp::initTestNav ()
{
  tp = new TestPage(baseWidget);
  QObject::connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addWidget(tp, 4);
}

void QtstalkerApp::initIndicatorNav ()
{
  ip = new IndicatorPage(baseWidget);
  connect(this, SIGNAL(signalIndicatorPageRefresh()), ip, SLOT(refreshList()));
  connect(ip, SIGNAL(signalDisableIndicator(QString)), this, SLOT(slotDisableIndicator(QString)));
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(slotEnableIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  connect(ip, SIGNAL(signalEditIndicator(QString)), this, SLOT(slotEditIndicator(QString)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  
  navTab->addWidget(ip, 2);
}

void QtstalkerApp::initScannerNav ()
{
  sp = new ScannerPage(baseWidget);
  QObject::connect(sp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addWidget(sp, 5);
}

void QtstalkerApp::initMacroNav ()
{
  mp = new MacroPage(baseWidget);
//  QObject::connect(mp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addWidget(mp, 6);
}

void QtstalkerApp::slotHideNav (bool d)
{
  menubar->setStatus("sidePanel", d);

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
  menubar->setStatus("hideMain", d);
  
  mainPlot->setHideMainPlot(d);
  mainPlot->draw();
}

void QtstalkerApp::slotPlotDate (bool d)
{
  menubar->setStatus("indicatorDate", d);

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
    mainPlot->crossHair(x, y, TRUE);

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->crossHair(x, y, TRUE);
    else
      it.current()->crossHair(x, y, FALSE);
  }
  
  slotTabChanged(0);
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
}

void QtstalkerApp::slotPlotKeyPressed (QKeyEvent *)
{
  if (status == None)
    return;

// FIXME    
/*    
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
*/  
}

void QtstalkerApp::slotDrawMode (bool d)
{
  emit signalDrawMode(d);
}

void QtstalkerApp::setSliderStart (int ov, bool flag)
{
  if (status == None)
    return;

  int rc = toolbar2->setSliderStart(ov, flag, mainPlot->getWidth(), recordList->count());
  emit signalIndex(rc);
}

void QtstalkerApp::slotNavigatorPosition (int d)
{
  if (! d)
    navSplitter->moveToFirst(navBase);
  else
    navSplitter->moveToLast(navBase);
    
  navSplitter->refresh();
}

void QtstalkerApp::slotMainPlotFocus ()
{
  mainPlot->setFocus();
}

void QtstalkerApp::slotTabIndicatorFocus ()
{
  tabs->setFocus();
}

// this is called by the bg for the panel buttons
void QtstalkerApp::slotNavigatorButtonPressed (int id)
{
  navTab->raiseWidget(id);

  switch (id)
  {
    case 0: // chart page
      chartNav->setNavFocus();
      break;
    case 1: // group page
      gp->setNavFocus();
      break;
    case 2: // indicator page
      ip->setFocus();
      break;
    case 3: // portfolio page
      pp->setFocus();
      break;
    case 4: // test page
      tp->setFocus();
      break;
    case 5: // scanner page
      sp->setFocus();
      break;
    case 6: // macro page
      mp->setFocus();
      break;
    default:
      break;
  }
}

void QtstalkerApp::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "toc.html");
  hw->show();
}

void QtstalkerApp::slotExitQuoteDialog ()
{
  quoteDialog = 0;
}

/*
// this is called by the qaction for CTRL+Key_0
void QtstalkerApp::slotChartPanelFocus ()
{
  if (bg->find(0)->isOn())
    return;
    
  bg->find(0)->toggle();
  navTab->raiseWidget(0);
  chartNav->setNavFocus();
}

// this is called by the qaction for CTRL+Key_1
void QtstalkerApp::slotGroupPanelFocus ()
{
  if (bg->find(1)->isOn())
    return;
    
  bg->find(1)->toggle();
  navTab->raiseWidget(1);
  gp->setNavFocus();
}

// this is called by the qaction for CTRL+Key_2
void QtstalkerApp::slotIndicatorPanelFocus ()
{
  if (bg->find(2)->isOn())
    return;
    
  bg->find(2)->toggle();
  navTab->raiseWidget(2);
  ip->setFocus();
}

// this is called by the qaction for CTRL+Key_3
void QtstalkerApp::slotPortfolioPanelFocus ()
{
  if (bg->find(3)->isOn())
    return;
    
  bg->find(3)->toggle();
  navTab->raiseWidget(3);
  pp->setFocus();
}

// this is called by the qaction for CTRL+Key_4
void QtstalkerApp::slotTesterPanelFocus ()
{
  if (bg->find(4)->isOn())
    return;
    
  bg->find(4)->toggle();
  navTab->raiseWidget(4);
  tp->setFocus();
}

// this is called by the qaction for CTRL+Key_5
void QtstalkerApp::slotScannerPanelFocus ()
{
  if (bg->find(5)->isOn())
    return;
    
  bg->find(5)->toggle();
  navTab->raiseWidget(5);
  sp->setFocus();
}

// this is called by the qaction for CTRL+Key_6
void QtstalkerApp::slotMacroPanelFocus ()
{
  if (bg->find(6)->isOn())
    return;
    
  bg->find(6)->toggle();
  navTab->raiseWidget(6);
  mp->setFocus();
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




