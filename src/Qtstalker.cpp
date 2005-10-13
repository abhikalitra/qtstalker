
/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2005 by Stefan Stratigakos
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
#include <qstringlist.h>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "HelpWindow.h"
#include "MacroKey.h"
#include "DbPlugin.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp()
{
  recordList = 0;
  quoteDialog = 0;
  status = None;
  plotList.setAutoDelete(TRUE);
  setIcon(qtstalker);
//  currentMacro = 0;

  // setup the disk environment
  config.setup();
  
  initMenuBar();
  
  initToolBar();
  
  statusbar = statusBar();
  
  baseWidget = new QWidget(this);
  setCentralWidget (baseWidget);

  QHBoxLayout *hbox = new QHBoxLayout(baseWidget);

  navSplitter = new QSplitter(baseWidget);
  navSplitter->setOrientation(Horizontal);
  hbox->addWidget(navSplitter);
  
  navBase = new QWidget(navSplitter);
  QVBoxLayout *vbox = new QVBoxLayout(navBase);
  
  // setup the data panel splitter
  dpSplitter = new QSplitter(navBase);
  dpSplitter->setOrientation(Vertical);
  vbox->addWidget(dpSplitter);
  
  // setup the side panels
  navTab = new NavigatorTab(dpSplitter, this);
  connect(navTab, SIGNAL(signalPositionChanged(int)), this, SLOT(slotNavigatorPosition(int)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), navTab, SLOT(setKeyFlag(bool)));
  
  // setup the data panel area
  infoLabel = new QMultiLineEdit(dpSplitter);
  infoLabel->setReadOnly(TRUE);

  // construct the chart areas
  QWidget *chartBase = new QWidget(navSplitter);

  vbox = new QVBoxLayout(chartBase);

  split = new QSplitter(chartBase);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  // setup the main plot
  mainPlot = new Plot (split);
  mainPlot->setDateFlag(TRUE);
  mainPlot->setMainFlag(TRUE);
  mainPlot->setLogScale(menubar->getStatus(MainMenubar::Log));
  connect(menubar, SIGNAL(signalLog(bool)), mainPlot, SLOT(slotLogScaleChanged(bool)));
  connect(menubar, SIGNAL(signalHideMain(bool)), mainPlot->getIndicatorPlot(), SLOT(slotHideMainChanged(bool)));

  // setup the indicator tabs
  tabs = new IndicatorTab(split);

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
    
  // create the side panels
  initChartNav();
  initGroupNav();
  initIndicatorNav();
  initPortfolioNav();
  initTestNav();
  initScannerNav();
//  initMacroNav();

  // set up the mainPlot signals
  initPlot(mainPlot);

  // setup the initial indicators
  QString igroup = config.getData(Config::IndicatorGroup);
  QStringList l;
  config.getIndicators(igroup, l);
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
    slotEnableIndicator(l[loop]);

  // set the app font
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
  QStringList l2 = QStringList::split(",", config.getData(Config::PlotSizes), FALSE);
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

  // restore the size of the app
  resize(config.getData(Config::Width).toInt(), config.getData(Config::Height).toInt());
  
  // restore the side panel position
  navTab->togglePosition(navTab->getPosition());

  // setup the indicator page  
  ip->updateList();
  
  // make sure the focus is set to the chart panel
  navTab->buttonPressed(0);
  
  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));
  
  progBar = new QProgressBar(this);
  statusbar->addWidget(progBar, 0, TRUE);
  progBar->setMaximumHeight(progBar->height() - 10);

  statusbar->message(tr("Ready"), 2000);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initMenuBar()
{
  // create the main menubar
  menubar = new MainMenubar(this);
  connect(menubar, SIGNAL(signalExit()), qApp, SLOT(quit()));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), menubar, SLOT(setKeyFlag(bool)));
//  connect(menubar, SIGNAL(signalRunMacro(QString)), this, SLOT(slotRunMacro(QString)));
  connect(menubar, SIGNAL(signalSidePanel(bool)), this, SLOT(slotHideNav(bool)));
  connect(menubar, SIGNAL(signalOptions()), this, SLOT(slotOptions()));
  connect(menubar, SIGNAL(signalQuotes()), this, SLOT(slotQuotes()));
}

void QtstalkerApp::initToolBar()
{
  // construct the button toolbar
  toolbar = new QToolBar(this, "buttonToolbar");
  menubar->getAction(MainMenubar::Exit)->addTo(toolbar);
  menubar->getAction(MainMenubar::Options)->addTo(toolbar);
  menubar->getAction(MainMenubar::SidePanel)->addTo(toolbar);
  menubar->getAction(MainMenubar::Grid)->addTo(toolbar);
  menubar->getAction(MainMenubar::ScaleToScreen)->addTo(toolbar);
  menubar->getAction(MainMenubar::Log)->addTo(toolbar);
  menubar->getAction(MainMenubar::HideMain)->addTo(toolbar);
  menubar->getAction(MainMenubar::IndicatorDate)->addTo(toolbar);
  menubar->getAction(MainMenubar::DrawMode)->addTo(toolbar);
  menubar->getAction(MainMenubar::NewIndicator)->addTo(toolbar);
  menubar->getAction(MainMenubar::DataWindow)->addTo(toolbar);
  menubar->getAction(MainMenubar::Quotes)->addTo(toolbar);
  menubar->getAction(MainMenubar::Help)->addTo(toolbar);

  // construct the chart toolbar
  toolbar2 = new ChartToolbar(this);
  connect(toolbar2, SIGNAL(signalCompressionChanged(int)), this, SLOT(slotCompressionChanged(int)));
  connect(toolbar2, SIGNAL(signalChartTypeChanged(int)), this, SLOT(slotChartTypeChanged(int)));
  connect(toolbar2, SIGNAL(signalPixelspaceChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), toolbar2, SLOT(setKeyFlag(bool)));
  connect(toolbar2, SIGNAL(signalBarsChanged(int)), this, SLOT(slotChartUpdated()));
}

void QtstalkerApp::slotQuit()
{
  // save any chart data
  mainPlot->clear();
  
  // do this to save any pending chart object edits
  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();

  // save plot sizes
  QValueList<int> list = split->sizes();
  int loop;
  QStringList l;
  for (loop = 0; loop < (int) list.count(); loop++)
    l.append(QString::number(list[loop]));
  QString s(l.join(","));
  config.setData(Config::PlotSizes, s);
  
  // save data panel size
  list = dpSplitter->sizes();
  l.clear();
  for (loop = 0; loop < (int) list.count(); loop++)
    l.append(QString::number(list[loop]));
  s = l.join(",");
  config.setData(Config::DataPanelSize, s);
  
  s = QString::number(this->height());
  config.setData(Config::Height, s);
  s = QString::number(this->width());
  config.setData(Config::Width, s);
  s = QString::number(this->x());
  config.setData(Config::X, s);
  s = QString::number(this->y());
  config.setData(Config::Y, s);
  s = QString::number(mainPlot->getCrosshairsStatus());
  config.setData(Config::Crosshairs, s);
  s = ip->getIndicatorGroup();
  config.setData(Config::IndicatorGroup, s);
  config.closePlugins();
  
  // make sure we clean up the local indicators before we quit
  ip->removeLocalIndicators();
  
  menubar->saveSettings();
  toolbar2->saveSettings();

  // delete any BarData
  if (recordList)
    delete recordList;
    
  if (quoteDialog)
    delete quoteDialog;
    
//  if (currentMacro)
//    delete currentMacro;
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					tr("Qtstalker\nVer CVS 0.33 (working title)\n(C) 2001-2005 by Stefan Stratigakos"),
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
  // load a chart slot
  toolbar2->enableSlider(TRUE);
  status = Chart;
  qApp->processEvents();
  loadChart(selection);
}

void QtstalkerApp::slotQuotes ()
{
  // display the quotes dialog
  if (quoteDialog)
    quoteDialog->raise();
  else
  {
    quoteDialog = new QuoteDialog();
    connect(quoteDialog, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
    connect(quoteDialog, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
    connect(quoteDialog, SIGNAL(destroyed()), this, SLOT(slotExitQuoteDialog()));
    quoteDialog->show();
  }
}

void QtstalkerApp::slotOptions ()
{
  // display the prefs dialog
  PrefDialog *dialog = new PrefDialog;
  dialog->setCaption(tr("Edit Prefs"));
  QString s("preferences.html");
  dialog->setHelpFile(s);

  s = tr("General");
  QString s2(tr("Main Menubar"));
  dialog->createPage(s);
  dialog->addCheckItem(s2, s, config.getData(Config::Menubar).toInt());
  
  s = tr("Colors");
  dialog->createPage(s);
  s2 = tr("Chart Background");
  QColor c(config.getData(Config::BackgroundColor));
  dialog->addColorItem(s2, s, c);
  s2 = tr("Chart Border");
  c.setNamedColor(config.getData(Config::BorderColor));
  dialog->addColorItem(s2, s, c);
  s2 = tr("Chart Grid");
  c.setNamedColor(config.getData(Config::GridColor));
  dialog->addColorItem(s2, s, c);

  s = tr("Fonts");
  dialog->createPage(s);
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  if (l.count() == 3)
  {
    s2 = tr("Plot Font");
    QFont f(l[0], l[1].toInt(), l[2].toInt());
    dialog->addFontItem(s2, s, f);
  }
  l = QStringList::split(",", config.getData(Config::AppFont), FALSE);
  if (l.count() == 3)
  {
    s2 = tr("App Font");
    QFont f(l[0], l[1].toInt(), l[2].toInt());
    dialog->addFontItem(s2, s, f);
  }
  
  int rc = dialog->exec();

  if (rc == QDialog::Accepted)
  {
    // save the main menubar setting
    s = tr("Main Menubar");
    bool flag = dialog->getCheck(s);
    s = QString::number(flag);
    config.setData(Config::Menubar, s);
    if (flag)
      menubar->show();
    else
      menubar->hide();
  
    s = tr("Chart Background");
    c = dialog->getColor(s);
    s = c.name();
    config.setData(Config::BackgroundColor, s);
    emit signalBackgroundColor(c);

    s = tr("Chart Border");
    c = dialog->getColor(s);
    s = c.name();
    config.setData(Config::BorderColor, s);
    emit signalBorderColor(c);

    s = tr("Chart Grid");
    c = dialog->getColor(s);
    s = c.name();
    config.setData(Config::GridColor, s);
    emit signalGridColor(c);

    // save plot font option
    s = tr("Plot Font");
    QFont f(dialog->getFont(s));
    s = f.family();
    s.append(",");
    s.append(QString::number(f.pointSize()));
    s.append(",");
    s.append(QString::number(f.weight()));
    s2 = config.getData(Config::PlotFont);
    if (s.compare(s2))
    {
      config.setData(Config::PlotFont, s);
      emit signalPlotFont(f);
    }

    // save app font option
    s = tr("App Font");
    f = dialog->getFont(s);
    s = f.family();
    s.append(",");
    s.append(QString::number(f.pointSize()));
    s.append(",");
    s.append(QString::number(f.weight()));
    s2 = config.getData(Config::AppFont);
    if (s.compare(s2))
    {
      config.setData(Config::AppFont, s);
      qApp->setFont(f, TRUE, 0);
    }
    
    loadChart(chartPath);

    statusbar->message (tr("Preferences saved."));
  }

  delete dialog;
}

void QtstalkerApp::loadChart (QString &d)
{
  // do all the stuff we need to do to load a chart
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
  ip->setChartPath(chartPath); // let ip know what chart we are viewing currently
  
  mainPlot->clear();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();
    
  // make sure we change db's after Plot saves previous db edits
  emit signalChartPath(chartPath);

  QString plugin = config.parseDbPlugin(chartPath);
  DbPlugin *plug = config.getDbPlugin(plugin);
  if (! plug)
  {
    config.closePlugin(plugin);
    return;
  }
  
  if (plug->openChart(chartPath))
  {
    qDebug("QtstalkerApp::loadChart: can't open db");
    config.closePlugin(plugin);
    return;
  }
  
  connect(plug, SIGNAL(signalProgMessage(int, int)), this, SLOT(slotProgMessage(int, int)));
  connect(plug, SIGNAL(signalStatusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  
  plug->setBarCompression((BarData::BarCompression) toolbar2->getCompressionInt());
  plug->setBarRange(toolbar2->getBars());
  
  if (recordList)
    delete recordList;
  recordList = new BarData;
  
  slotStatusMessage(tr("Loading chart..."));
  
  plug->getHistory(recordList);
  
  slotProgMessage(1, 3);
  slotStatusMessage(tr("Loading indicators..."));
  
  plug->getHeaderField(DbPlugin::Title, chartName);
  plug->getHeaderField(DbPlugin::Type, chartType);
  plug->getHeaderField(DbPlugin::Symbol, chartSymbol);
  
  mainPlot->setData(recordList);
  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);
    
  mainPlot->setChartInput ();

  // setup the local indicators
  if (! reload)
  {
    ip->removeLocalIndicators();
    QString s;
    plug->getHeaderField(DbPlugin::LocalIndicators, s);
    ip->addLocalIndicators(s);
  }
  QStringList l;
  QString s(ip->getIndicatorGroup());
  config.getIndicators(s, l);

  int loop;  
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    Setting set;
    config.getIndicator(l[loop], set);
    if (! set.count() || ! set.getInt("enable"))
      continue;
    
    Indicator *i = new Indicator;
    QFileInfo fi(l[loop]);
    QString str = fi.fileName();
    i->setName(str);
    i->setFile(l[loop]);
    str = set.getData("plugin");
    i->setType(str);
    if (set.getData("plotType").length())
      i->setPlotType((Indicator::PlotType) set.getData("plotType").toInt());
    loadIndicator(i);
  }
  
  slotProgMessage(2, 3);
  slotStatusMessage(tr("Loading chart objects..."));

  plug->getChartObjects(l);
  Setting co;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    co.parse(l[loop]);
    QString s = co.getData("Plot");
    if (! s.compare(tr("Main Plot")))
      mainPlot->addChartObject(co);
    else
    {
      Plot *plot = plotList[s];
      if (plot)
        plot->addChartObject(co);
    }
  }
  
  config.closePlugin(plugin);

  // update the pixelspace
  toolbar2->setMinPixelspace(mainPlot->getMinPixelspace());
  
  setSliderStart(toolbar2->getPixelspace(), TRUE);
  
  mainPlot->draw();

  for(it.toFirst(); it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  tabs->drawCurrent();
  
  ip->updateList();

  setCaption(getWindowCaption());
  
  slotProgMessage(-1, 3);
  slotStatusMessage(QString());
}

void QtstalkerApp::loadIndicator (Indicator *i)
{
  // create and prep an indicator for display

  QString plugin(i->getType());  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (plug)
  {
    plug->clearOutput();
    if (recordList)
      plug->setIndicatorInput(recordList);
    QString s(i->getFile());
    plug->loadIndicatorSettings(s);
    if (recordList)
      plug->calculate();
    i->copy(plug->getIndicator());

    s = i->getName();
    if (i->getPlotType() == Indicator::MainPlot)
      mainPlot->addIndicator(s, i);
    else
    {
      Plot *plot = plotList[s];
      plot->addIndicator(s, i);
      if (recordList)
        plot->setData(recordList);
    }
  }
  
//  config.closePlugin(plugin);
}

QString QtstalkerApp::getWindowCaption ()
{
  // update the main window text
  
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
  // show the datawindow dialog
  
  if (! recordList)
    return;
  
  DataWindow *dw = new DataWindow();
  dw->setCaption(getWindowCaption());

  dw->setBars(recordList);
  
  dw->setPlot(mainPlot);
  
  QDictIterator<Plot> it(plotList);
  for (; it.current(); ++it)
    dw->setPlot(it.current());
  
  dw->show();
}

void QtstalkerApp::slotCompressionChanged (int)
{
  // the compression has changed slot
  
  compressionChanged();
  loadChart(chartPath);
}

void QtstalkerApp::compressionChanged ()
{
  // the compression has changed

  QString s = QString::number(toolbar2->getCompressionInt());
  config.setData(Config::Compression, s);

  emit signalInterval((BarData::BarCompression) toolbar2->getCompressionInt());
}

void QtstalkerApp::slotChartTypeChanged (int)
{
  // the chart type has changed

  QString s = toolbar2->getChartType();
  if (mainPlot->setChartType(s))
    return;

  emit signalPixelspace(mainPlot->getPixelspace());

  toolbar2->setPixelspace(mainPlot->getMinPixelspace(), mainPlot->getPixelspace());
  
  setSliderStart(toolbar2->getPixelspace(), TRUE);
  
  emit signalIndex(toolbar2->getSlider());
  
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  tabs->drawCurrent();
}

void QtstalkerApp::slotNewIndicator (Indicator *i)
{
  // add a new indicator slot
  
  QString str = i->getFile();
  addIndicatorButton(str, i->getPlotType());
  loadIndicator(i);

  if (i->getPlotType() == Indicator::MainPlot)
    mainPlot->draw();
  else
    tabs->drawCurrent();
    
  ip->updateList();
}

void QtstalkerApp::slotEditIndicator (Indicator *i)
{
  // edit indicator slot

  loadIndicator(i);
    
  if (i->getPlotType() == Indicator::MainPlot)
    mainPlot->draw();
  else
    tabs->drawCurrent();
}

void QtstalkerApp::slotDeleteIndicator (QString text)
{
  // delete indicator slot

  QString s = config.getData(Config::IndicatorPath) + "/" + ip->getIndicatorGroup() + "/" + text;
  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::slotDeleteIndicator:indicator settings empty");
    return;
  }

  // delete any chart objects that belong to the indicator
  bool mainFlag = FALSE;
  bool tabFlag = FALSE;
  switch (set.getInt("plotType"))
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
    tabs->deleteTab(text);
    plotList.remove(text);
  }
  else
    mainPlot->deleteIndicator(text);

  config.deleteIndicator(s);

  ip->updateList();
  
  if (mainFlag)
    mainPlot->draw();
}

void QtstalkerApp::slotDisableIndicator (QString name)
{
  // remove indicator

  QFileInfo fi(name);
  QString s = fi.fileName();
  if (mainPlot->deleteIndicator(s))
    mainPlot->draw();
  else
  {
    tabs->deleteTab(s);
    plotList.remove(s);
  }
}

void QtstalkerApp::slotEnableIndicator (QString name)
{
  // add indicator

  Setting set;
  config.getIndicator(name, set);
  if (! set.count())
    return;
    
  if (set.getInt("enable") == 0)
    return;
  
  addIndicatorButton(name, (Indicator::PlotType) set.getData("plotType").toInt());
  
  Indicator *i = new Indicator;
  QFileInfo fi(name);
  QString str = fi.fileName();
  i->setName(str);
  i->setFile(name);
  str = set.getData("plugin");
  i->setType(str);
  if (set.getData("plotType").length())
    i->setPlotType((Indicator::PlotType) set.getData("plotType").toInt());
  loadIndicator(i);
  
  if (i->getPlotType() == Indicator::MainPlot)
    mainPlot->draw();
  else
  {
    QDictIterator<Plot> it(plotList);
    for(; it.current(); ++it)
    {
      if (! it.current()->getTabFlag())
        it.current()->draw();
    }
    
    tabs->drawCurrent();
  }
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  emit signalPixelspace(d);
  
  emit signalIndex(toolbar2->getSlider());
  
  mainPlot->draw();

  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
  {
    if (! it.current()->getTabFlag())
      it.current()->draw();
  }

  tabs->drawCurrent();
}

void QtstalkerApp::slotMinPixelspaceChanged (int d)
{
  toolbar2->setPixelspace(d, d);
  slotPixelspaceChanged(d);
}

void QtstalkerApp::addIndicatorButton (QString &d, Indicator::PlotType tabFlag)
{
  Setting set;
  config.getIndicator(d, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::addIndicatorButton:indicator settings empty");
    return;
  }
  
  QString type = set.getData("plugin");

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
    a[a.size() - 2] = 100;
    split->setSizes(a);
  }
  
  QFileInfo fi(d);
  plotList.replace(fi.fileName(), plot);
  
  initPlot(plot);

  plot->setDateFlag(menubar->getStatus(MainMenubar::IndicatorDate));
  connect(menubar, SIGNAL(signalPlotDate(bool)), plot, SLOT(slotDateFlagChanged(bool)));

  if (tabFlag == Indicator::TabPlot)
  {
    QString t = fi.fileName();
    int page = tabs->getInsertIndex(t);
    tabs->insertTab(plot, fi.fileName(), page);
  }
}

void QtstalkerApp::initPlot (Plot *plot)
{
  QColor color;
  color.setNamedColor(config.getData(Config::BackgroundColor));
  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  color.setNamedColor(config.getData(Config::BorderColor));
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  color.setNamedColor(config.getData(Config::GridColor));
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  QStringList l = QStringList::split(",", config.getData(Config::PlotFont), FALSE);
  if (l.count() == 3)
  {
    QFont font(l[0], l[1].toInt(), l[2].toInt());
    plot->setPlotFont(font);
  }
  else
  {
    QFont f;
    plot->setPlotFont(f);
  }

  plot->setGridFlag(menubar->getStatus(MainMenubar::Grid));
  plot->setScaleToScreen(menubar->getStatus(MainMenubar::ScaleToScreen));
  plot->setPixelspace(toolbar2->getPixelspace());
  plot->setIndex(toolbar2->getSlider());
  plot->setInterval((BarData::BarCompression) toolbar2->getCompressionInt());
  plot->setCrosshairsStatus(config.getData(Config::Crosshairs).toInt());  
  plot->setDrawMode(menubar->getStatus(MainMenubar::DrawMode));
    
  connect(plot->getIndicatorPlot(), SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(plot->getIndicatorPlot(), SIGNAL(signalEditIndicator(QString)), ip, SLOT(editIndicator(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  connect(plot->getIndicatorPlot(), SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalMinPixelspace(int)), this, SLOT(slotMinPixelspaceChanged(int)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalCrosshairsStatus(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot->getIndicatorPlot(), SLOT(setCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarCompression)), plot, SLOT(setInterval(BarData::BarCompression)));
  connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));
  
  connect(toolbar2, SIGNAL(signalSliderChanged(int)), plot, SLOT(slotSliderChanged(int)));
  connect(menubar, SIGNAL(signalGrid(bool)), plot->getIndicatorPlot(), SLOT(slotGridChanged(bool)));
  connect(menubar, SIGNAL(signalScale(bool)), plot, SLOT(slotScaleToScreenChanged(bool)));
  connect(menubar, SIGNAL(signalDraw(bool)), plot->getIndicatorPlot(), SLOT(slotDrawModeChanged(bool)));
}

void QtstalkerApp::slotChartUpdated ()
{
  chartNav->refreshList();
  
  if (status == None)
    return;
  
  QString s = QString::number(toolbar2->getBars());
  config.setData(Config::Bars, s);

  loadChart(chartPath);
}

void QtstalkerApp::slotStatusMessage (QString d)
{
  statusbar->message(d);
  qApp->processEvents();
}

void QtstalkerApp::initGroupNav ()
{
  gp = new GroupPage(baseWidget);
  connect(gp, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), gp, SLOT(setKeyFlag(bool)));
  navTab->addWidget(gp, 1);
}

void QtstalkerApp::initChartNav ()
{
  chartNav = new ChartPage(baseWidget);
  connect(chartNav, SIGNAL(fileSelected(QString)), this, SLOT(slotOpenChart(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), chartNav, SLOT(setKeyFlag(bool)));
  navTab->addWidget(chartNav, 0);
}

void QtstalkerApp::initPortfolioNav ()
{
  pp = new PortfolioPage(baseWidget);
  connect(this, SIGNAL(signalSetKeyFlag(bool)), pp, SLOT(setKeyFlag(bool)));
  navTab->addWidget(pp, 3);
}

void QtstalkerApp::initTestNav ()
{
  tp = new TestPage(baseWidget, menubar);
  connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), tp, SLOT(setKeyFlag(bool)));
  navTab->addWidget(tp, 4);
}

void QtstalkerApp::initIndicatorNav ()
{
  ip = new IndicatorPage(baseWidget);
  connect(ip, SIGNAL(signalDisableIndicator(QString)), this, SLOT(slotDisableIndicator(QString)));
  connect(ip, SIGNAL(signalEnableIndicator(QString)), this, SLOT(slotEnableIndicator(QString)));
  connect(ip, SIGNAL(signalNewIndicator(Indicator *)), this, SLOT(slotNewIndicator(Indicator *)));
  connect(ip, SIGNAL(signalEditIndicator(Indicator *)), this, SLOT(slotEditIndicator(Indicator *)));
  connect(ip, SIGNAL(signalDeleteIndicator(QString)), this, SLOT(slotDeleteIndicator(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), ip, SLOT(setKeyFlag(bool)));
  connect(menubar, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  navTab->addWidget(ip, 2);
}

void QtstalkerApp::initScannerNav ()
{
  sp = new ScannerPage(baseWidget);
  connect(sp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  navTab->addWidget(sp, 5);
}

void QtstalkerApp::slotHideNav (bool d)
{
  menubar->setStatus(MainMenubar::SidePanel, d);

  if (d)
    navBase->show();
  else
    navBase->hide();
}

void QtstalkerApp::slotUpdateInfo (Setting *r)
{
  QStringList l;
  r->getKeyList(l);
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
  
  tabs->drawCurrent();
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
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

void QtstalkerApp::slotHelp ()
{
  QString s = "toc.html";
  HelpWindow *hw = new HelpWindow(this, s);
  hw->show();
}

void QtstalkerApp::slotExitQuoteDialog ()
{
  quoteDialog = 0;
}

//***************************************************************
//*********** MACRO FUNCTIONS ***********************************
//***************************************************************

/*
void QtstalkerApp::initMacroNav ()
{
  mp = new MacroPage(baseWidget);
  connect(mp, SIGNAL(signalRunMacro(QString)), this, SLOT(slotRunMacro(QString)));
  connect(mp, SIGNAL(signalRecordMacro(QString)), this, SLOT(slotRecordMacro(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), mp, SLOT(setKeyFlag(bool)));
  navTab->addWidget(mp, 6);
}
*/

/*
void QtstalkerApp::slotRunMacro (QString d)
{
  slotStatusMessage(tr("Running macro..."));

  Macro *m = new Macro(d);
  m->load();
  m->setIndex(0);
  
  while (m->getIndex() < m->getCount())
  {
    switch(m->getZone(m->getIndex()))
    {
      case Macro::ChartPage:
        chartNav->runMacro(m);
	break;
      case Macro::GroupPage:
        gp->runMacro(m);
	break;
      case Macro::IndicatorPage:
        ip->runMacro(m);
	break;
      case Macro::PortfolioPage:
        pp->runMacro(m);
	break;
      case Macro::TestPage:
        tp->runMacro(m);
	break;
      case Macro::ScannerPage:
        sp->runMacro(m);
	break;
      case Macro::MacroPage:
        mp->runMacro(m);
	break;
      case Macro::SidePanel:
        navTab->runMacro(m);
	break;
      case Macro::ChartToolbar:
        toolbar2->runMacro(m);
	break;
      case Macro::Menubar:
        menubar->runMacro(m);
	break;
      default:
        break;
    }
  }
  
  delete m;
  
  slotStatusMessage(tr("Macro finished"));
}
*/
/*
void QtstalkerApp::slotRecordMacro (QString d)
{
  QString s = tr("Macro recording session about to begin.\n");
  s.append(tr("Press CTRL+ESC to stop recording.\n\n"));
  s.append(tr("Proceed?"));
  
  int rc = QMessageBox::warning(this,
    			        tr("Qtstalker: Warning"),
			        s,
			        QMessageBox::Yes,
			        QMessageBox::No,
			        QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  if (currentMacro)
    delete currentMacro;
    
  currentMacro = new Macro(d);
  connect(menubar, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(ip, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(chartNav, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(gp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(pp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(tp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(sp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(mp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(toolbar2, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(navTab, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  
  emit signalSetKeyFlag(TRUE);
  
  currentMacro->record();
  
  slotStatusMessage(tr("Recording macro session..."));
}
*/

/*
void QtstalkerApp::slotStopMacro ()
{
  if (! currentMacro)
    return;
    
  emit signalSetKeyFlag(FALSE);
    
  currentMacro->stop();
  delete currentMacro;
  currentMacro = 0;
  mp->updateList();
  slotStatusMessage(tr("Recording macro finished."));
  QMessageBox::information(this, tr("Qtstalker:Info"), tr("Macro session ended."));
}
*/

void QtstalkerApp::slotProgMessage (int p, int t)
{
  if (p == -1)
    progBar->reset();
  else
    progBar->setProgress(p, t);
    
  qApp->processEvents();
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
//  tor.load(QString("qtstalker_") + QTextCodec::locale(), "." );
  tor.load(QString("qtstalker_") + QTextCodec::locale(), "/usr/share/qtstalker/i18n" );
  a.installTranslator( &tor );

  QtstalkerApp *qtstalker = new QtstalkerApp();
  a.setMainWidget(qtstalker);

  qtstalker->show();

  return a.exec();
}




