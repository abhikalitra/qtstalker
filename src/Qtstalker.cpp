
/***************************************************************************
                          qtstalker.cpp  -  description
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2006 by Stefan Stratigakos
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
#include <qdatetime.h>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "HelpWindow.h"
#include "DbPlugin.h"

#include "../pics/qtstalker.xpm"

QtstalkerApp::QtstalkerApp()
{
  recordList = 0;
  status = None;
  plotList.setAutoDelete(FALSE);
  tabList.setAutoDelete(TRUE);
  widgetList.setAutoDelete(TRUE);
  setIcon(qtstalker);

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
  
  // setup the data panel area
  infoLabel = new QMultiLineEdit(dpSplitter);
  infoLabel->setReadOnly(TRUE);

  // construct the chart areas
  QWidget *chartBase = new QWidget(navSplitter);

  vbox = new QVBoxLayout(chartBase);

  split = new QSplitter(chartBase);
  split->setOrientation(Vertical);
  vbox->addWidget(split);

  // build the tab rows
  int loop;
  for (loop = 0; loop < 3; loop++)
  {
    QTabWidget *it = new QTabWidget(split);
    connect(it, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotDrawPlots()));
    tabList.append(it);
    it->hide();
  }

  // set the nav splitter size
  QString s, s2;
  config.getData(Config::NavAreaSize, s);
  QValueList<int> sizeList;
  sizeList.append(s.toInt());
  config.getData(Config::Width, s2);
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

  // setup the initial indicators
  QString igroup;
  config.getData(Config::IndicatorGroup, igroup);
  QStringList l;
  config.getIndicators(igroup, l);
  for (loop = 0; loop < (int) l.count(); loop++)
    addIndicatorButton(l[loop]);

  // set the app font
  config.getData(Config::AppFont, s);
  l = QStringList::split(",", s, FALSE);
  if (l.count() == 3)
  {
    QFont font2(l[0], l[1].toInt(), l[2].toInt());
    qApp->setFont(font2, TRUE, 0);
  }
  else
    qApp->setFont(QFont(), TRUE, 0);
  
  // set the indicator splitter sizes
  sizeList = split->sizes();
  config.getData(Config::PlotSizes, s);
  QStringList l2 = QStringList::split(",", s, FALSE);
  for (loop = 0; loop < (int) l2.count(); loop++)
    sizeList[loop] = l2[loop].toInt();
  split->setSizes(sizeList);
  
  // set the data panel splitter size
  sizeList = dpSplitter->sizes();
  config.getData(Config::DataPanelSize, s);
  l = QStringList::split(",", s, FALSE);
  for (loop = 0; loop < (int) l.count(); loop++)
    sizeList[loop] = l[loop].toInt();
  dpSplitter->setSizes(sizeList);

  
  // set the nav status
  slotHideNav(TRUE);

  // restore the size of the app
  config.getData(Config::Width, s);
  config.getData(Config::Height, s2);
  resize(s.toInt(), s2.toInt());
  
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
  connect(menubar, SIGNAL(signalSidePanel(bool)), this, SLOT(slotHideNav(bool)));
  connect(menubar, SIGNAL(signalOptions()), this, SLOT(slotOptions()));
  connect(menubar, SIGNAL(signalQuotes()), this, SLOT(slotQuotes()));
  connect(menubar, SIGNAL(signalCrosshairs(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  connect(menubar, SIGNAL(signalPaperTrade(bool)), this, SLOT(slotPaperTradeChanged(bool)));
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
  menubar->getAction(MainMenubar::Crosshairs)->addTo(toolbar);
  menubar->getAction(MainMenubar::PaperTrade)->addTo(toolbar);
  menubar->getAction(MainMenubar::DrawMode)->addTo(toolbar);
  menubar->getAction(MainMenubar::NewIndicator)->addTo(toolbar);
  menubar->getAction(MainMenubar::DataWindow)->addTo(toolbar);
  menubar->getAction(MainMenubar::Quotes)->addTo(toolbar);
  menubar->getAction(MainMenubar::Help)->addTo(toolbar);

  // construct the chart toolbar
  toolbar2 = new ChartToolbar(this);
  connect(toolbar2, SIGNAL(signalBarLengthChanged(int)), this, SLOT(slotBarLengthChanged(int)));
  connect(toolbar2, SIGNAL(signalPixelspaceChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  connect(toolbar2, SIGNAL(signalBarsChanged(int)), this, SLOT(slotChartUpdated()));
  
  connect(toolbar2, SIGNAL(signalPaperTradeNextBar()), this, SLOT(slotChartUpdated()));
  toolbar2->paperTradeClicked(menubar->getStatus(MainMenubar::PaperTrade));

  connect(menubar, SIGNAL(signalAdvancePaperTrade()), toolbar2, SLOT(paperTradeNextBar()));
}

void QtstalkerApp::slotQuit()
{
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
}

void QtstalkerApp::slotAbout()
{
  // display the about dialog
  QMessageBox *dialog = new QMessageBox(tr("About Qtstalker"),
  					tr("Qtstalker\nVer CVS 0.34 (working title)\n(C) 2001-2006 by Stefan Stratigakos"),
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
  QStringList l;
  config.getPluginList(Config::QuotePluginPath, l);

  bool ok;
  QString s = QInputDialog::getItem (tr("Quote Dialog"), tr("Select Quote Type"), l, 0, FALSE, &ok, this, 0);
  if (! ok)
    return;

  QuotePlugin *plug = config.getQuotePlugin(s);
  if (! plug)
    return;

  connect(plug, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
  connect(plug, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));

  plug->show();
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
  QString ts;
  config.getData(Config::Menubar, ts);
  dialog->addCheckItem(s2, s, ts.toInt());
  
  s = tr("Colors");
  dialog->createPage(s);
  s2 = tr("Chart Background");
  config.getData(Config::BackgroundColor, ts);
  QColor c(ts);
  dialog->addColorItem(s2, s, c);
  s2 = tr("Chart Border");
  config.getData(Config::BorderColor, ts);
  c.setNamedColor(ts);
  dialog->addColorItem(s2, s, c);
  s2 = tr("Chart Grid");
  config.getData(Config::GridColor, ts);
  c.setNamedColor(ts);
  dialog->addColorItem(s2, s, c);

  s = tr("Fonts");
  dialog->createPage(s);
  config.getData(Config::PlotFont, ts);
  QStringList l = QStringList::split(",", ts, FALSE);
  if (l.count() == 3)
  {
    s2 = tr("Plot Font");
    QFont f(l[0], l[1].toInt(), l[2].toInt());
    dialog->addFontItem(s2, s, f);
  }
  config.getData(Config::AppFont, ts);
  l = QStringList::split(",", ts, FALSE);
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
    dialog->getColor(s, c);
    s = c.name();
    config.setData(Config::BackgroundColor, s);
    emit signalBackgroundColor(c);

    s = tr("Chart Border");
    dialog->getColor(s, c);
    s = c.name();
    config.setData(Config::BorderColor, s);
    emit signalBorderColor(c);

    s = tr("Chart Grid");
    dialog->getColor(s, c);
    s = c.name();
    config.setData(Config::GridColor, s);
    emit signalGridColor(c);

    // save plot font option
    s = tr("Plot Font");
    QFont f;
    dialog->getFont(s, f);
    s = f.family();
    s.append(",");
    s.append(QString::number(f.pointSize()));
    s.append(",");
    s.append(QString::number(f.weight()));
    config.getData(Config::PlotFont, s2);
    if (s.compare(s2))
    {
      config.setData(Config::PlotFont, s);
      emit signalPlotFont(f);
    }

    // save app font option
    s = tr("App Font");
    dialog->getFont(s, f);
    s = f.family();
    s.append(",");
    s.append(QString::number(f.pointSize()));
    s.append(",");
    s.append(QString::number(f.weight()));
    config.getData(Config::AppFont, s2);
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
  
  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->clear();
    
  // make sure we change db's after Plot saves previous db edits
  emit signalChartPath(chartPath);

  DbPlugin plug;
  if (plug.openChart(chartPath))
  {
    qDebug("QtstalkerApp::loadChart: can't open db");
    plug.close();
    return;
  }
  
  connect(&plug, SIGNAL(signalProgMessage(int, int)), this, SLOT(slotProgMessage(int, int)));
  connect(&plug, SIGNAL(signalStatusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  
  plug.setBarLength((BarData::BarLength) toolbar2->getBarLengthInt());
  plug.setBarRange(toolbar2->getBars());
  
  if (recordList)
    delete recordList;
  recordList = new BarData(chartPath);
  
  slotStatusMessage(tr("Loading chart..."));
  
  QDateTime dt = QDateTime::currentDateTime();
  if (menubar->getStatus(MainMenubar::PaperTrade))
    toolbar2->getPaperTradeDate(dt);
  plug.getHistory(recordList, dt);

  slotProgMessage(1, 3);
  slotStatusMessage(tr("Loading indicators..."));
  
  plug.getHeaderField(DbPlugin::Title, chartName);
  plug.getHeaderField(DbPlugin::Type, chartType);
  plug.getHeaderField(DbPlugin::Symbol, chartSymbol);
  
  for(it.toFirst(); it.current(); ++it)
    it.current()->setData(recordList);

  // setup the local indicators
  if (! reload)
  {
    ip->removeLocalIndicators();
    QString s;
    plug.getHeaderField(DbPlugin::LocalIndicators, s);
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
    if (! set.count())
      continue;
    
    Indicator *i = new Indicator;
    i->setIndicator(set, l[loop]);
    if (! i->getEnable())
    {
      delete i;
      continue;
    }

    loadIndicator(i);
  }
  
  slotProgMessage(2, 3);
  slotStatusMessage(tr("Loading chart objects..."));

  plug.getChartObjects(l);
  Setting co;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    co.parse(l[loop]);
    s = "Plot";
    QString s2;
    co.getData(s, s2);
    Plot *plot = plotList[s2];
    if (plot)
      plot->addChartObject(co);
  }
  
  plug.close();

  setSliderStart();

  slotDrawPlots();

  ip->updateList();

  setCaption(getWindowCaption());
  
  slotProgMessage(-1, 3);
  slotStatusMessage(QString());
}

void QtstalkerApp::loadIndicator (Indicator *i)
{
  // create and prep an indicator for display

  QString plugin;
  i->getType(plugin);  
  IndicatorPlugin *plug = config.getIndicatorPlugin(plugin);
  if (plug)
  {
    connect(plug, SIGNAL(signalWakeup()), this, SLOT(slotWakeup()));
    plug->clearOutput();
    if (recordList)
      plug->setIndicatorInput(recordList);
    QString s;
    i->getFile(s);
    plug->loadIndicatorSettings(s);
    if (recordList)
      plug->calculate();
    i->copy(plug->getIndicator());

    QFileInfo fi(s);
    Plot *plot = plotList[fi.fileName()];
    if (! plot)
    {
      qDebug("QtstalkerApp::loadIndicator:plot %s not found", fi.fileName().latin1());
      return;
    }
    plot->addIndicator(i);
    if (recordList)
      plot->setData(recordList);
  }
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

  caption.append(toolbar2->getBarLength());

  return caption;
}

void QtstalkerApp::slotDataWindow ()
{
  // show the datawindow dialog
  
  if (! recordList)
    return;
  
  DataWindow *dw = new DataWindow(0);
  dw->setCaption(getWindowCaption());

  dw->setBars(recordList);
  
  QDictIterator<Plot> it(plotList);
  for (; it.current(); ++it)
    dw->setPlot(it.current());
  
  dw->show();
}

void QtstalkerApp::slotBarLengthChanged (int)
{
  // the compression has changed slot
  
  barLengthChanged();
  loadChart(chartPath);
}

void QtstalkerApp::barLengthChanged ()
{
  // bar length has changed

  QString s = QString::number(toolbar2->getBarLengthInt());
  config.setData(Config::BarLength, s);

  emit signalInterval((BarData::BarLength) toolbar2->getBarLengthInt());
}

void QtstalkerApp::slotNewIndicator (Indicator *i)
{
  // add a new indicator slot
  
  QString str;
  i->getFile(str);
  addIndicatorButton(str);
  loadIndicator(i);
  ip->updateList();
}

void QtstalkerApp::slotEditIndicator (Indicator *i)
{
  // edit indicator slot

  loadIndicator(i);

  QString s;
  i->getFile(s);
  QFileInfo fi(s);
  Plot *p = plotList[fi.fileName()];
  if (p)
    p->draw();
}

void QtstalkerApp::slotDeleteIndicator (QString text)
{
  // delete indicator slot

  QString s;
  config.getData(Config::IndicatorPath, s);
  s.append("/" + ip->getIndicatorGroup() + "/" + text);
  Setting set;
  config.getIndicator(s, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::slotDeleteIndicator:indicator settings empty");
    return;
  }

  // delete any chart objects that belong to the indicator
  Indicator i;
  i.setIndicator(set, s);
  QTabWidget *it = tabList.at(i.getTabRow() - 1);
  it->removePage(widgetList[text]);
  widgetList.remove(text);

  if (! it->count())
    it->hide();

  plotList.remove(text);

  config.deleteIndicator(s);

  ip->updateList();
}

void QtstalkerApp::slotDisableIndicator (QString name)
{
  // remove indicator

  Setting set;
  config.getIndicator(name, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::slotDisableIndicator:indicator settings empty");
    return;
  }

  Indicator i;
  i.setIndicator(set, name);
  QTabWidget *it = tabList.at(i.getTabRow() - 1);
  QString s;
  i.getName(s);
  it->removePage(widgetList[s]);
  widgetList.remove(s);

  if (! it->count())
    it->hide();

  plotList.remove(s);
}

void QtstalkerApp::slotEnableIndicator (QString name)
{
  // add indicator

  Setting set;
  config.getIndicator(name, set);
  if (! set.count())
    return;

  Indicator *i = new Indicator;
  i->setIndicator(set, name);
  if (i->getEnable() == 0)
  {
    delete i;
    return;
  }
  
  addIndicatorButton(name);
  loadIndicator(i);
}

void QtstalkerApp::slotPixelspaceChanged (int d)
{
  emit signalPixelspace(d);
  emit signalIndex(toolbar2->getSlider());
  slotDrawPlots();
}

void QtstalkerApp::addIndicatorButton (QString d)
{
  Setting set;
  config.getIndicator(d, set);
  if (! set.count())
  {
    qDebug("QtstalkerApp::addIndicatorButton:indicator settings empty");
    return;
  }

  Indicator i;
  i.setIndicator(set, d);
  if (i.getEnable() == 0)
    return;
  
  QString type;
  i.getType(type);
  int row = i.getTabRow();

  QFileInfo fi(d);
  QString fn = fi.fileName();

  QWidget *w = new QWidget(baseWidget);
  QVBoxLayout *vbox = new QVBoxLayout(w);
  vbox->setMargin(0);
  vbox->setSpacing(0);
  Plot *plot = new Plot(w);
  vbox->addWidget(plot);
  widgetList.replace(fi.fileName(), w);
  
  plotList.replace(fi.fileName(), plot);

  QTabWidget *it = tabList.at(row - 1);
  it->addTab(w, fi.fileName());

  if (it->isHidden())
    it->show();

  QColor color;
  QString ts;
  config.getData(Config::BackgroundColor, ts);
  color.setNamedColor(ts);
  connect(this, SIGNAL(signalBackgroundColor(QColor)), plot, SLOT(setBackgroundColor(QColor)));
  plot->setBackgroundColor(color);

  config.getData(Config::BorderColor, ts);
  color.setNamedColor(ts);
  connect(this, SIGNAL(signalBorderColor(QColor)), plot, SLOT(setBorderColor(QColor)));
  plot->setBorderColor(color);

  config.getData(Config::GridColor, ts);
  color.setNamedColor(ts);
  connect(this, SIGNAL(signalGridColor(QColor)), plot, SLOT(setGridColor(QColor)));
  plot->setGridColor(color);

  connect(this, SIGNAL(signalPlotFont(QFont)), plot, SLOT(setPlotFont(QFont)));
  config.getData(Config::PlotFont, ts);
  QStringList l = QStringList::split(",", ts, FALSE);
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
  plot->setInterval((BarData::BarLength) toolbar2->getBarLengthInt());
  config.getData(Config::Crosshairs, ts);
  plot->setCrosshairsStatus(ts.toInt());  
  plot->setDrawMode(menubar->getStatus(MainMenubar::DrawMode));
    
  connect(plot->getIndicatorPlot(), SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(plot->getIndicatorPlot(), SIGNAL(signalEditIndicator(QString)), ip, SLOT(editIndicator(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(plot->getIndicatorPlot(), SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  connect(plot->getIndicatorPlot(), SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  connect(plot->getIndicatorPlot(), SIGNAL(signalEditChart(QString)), chartNav, SLOT(editChart(QString)));
  connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot->getIndicatorPlot(), SLOT(setCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarLength)), plot, SLOT(setInterval(BarData::BarLength)));
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
  connect(chartNav, SIGNAL(signalAddToGroup(QString)), gp, SLOT(addChartToGroup(QString)));
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
  tp = new TestPage(baseWidget, menubar);
  connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
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
  connect(menubar, SIGNAL(signalNewIndicator()), ip, SLOT(newIndicator()));
  connect(ip, SIGNAL(signalReloadChart()), this, SLOT(slotChartUpdated()));
  connect(ip, SIGNAL(signalLocalIndicator(QString)), this, SLOT(addIndicatorButton(QString)));
  navTab->addWidget(ip, 2);
}

void QtstalkerApp::initScannerNav ()
{
  sp = new ScannerPage(baseWidget);
  connect(sp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(sp, SIGNAL(refreshGroup()), gp, SLOT(refreshList()));
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
    str.append(l[loop] + " " + s + "\n");
  }

  delete r;

  infoLabel->setText(str);
}

void QtstalkerApp::slotPlotLeftMouseButton (int x, int y, bool)
{
  QDictIterator<Plot> it(plotList);
  for(; it.current(); ++it)
    it.current()->crossHair(x, y, FALSE);
  slotDrawPlots();  
}

void QtstalkerApp::slotCrosshairsStatus (bool status)
{
  emit signalCrosshairsStatus(status);
}

void QtstalkerApp::setSliderStart ()
{
  if (status == None)
    return;

  int loop;
  int rc = 0;
  for (loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *tw = tabList.at(loop);
    if (! tw->isHidden())
    {
      Plot *plot = plotList[tw->label(tw->currentPageIndex())];
      if (! plot)
        return;
      else
      {
        rc = toolbar2->setSliderStart(plot->getWidth(), recordList->count());
        break;
      }
    }
  }

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

void QtstalkerApp::slotProgMessage (int p, int t)
{
  if (p == -1)
    progBar->reset();
  else
    progBar->setProgress(p, t);
    
  qApp->processEvents();
}

void QtstalkerApp::slotDrawPlots ()
{
  int loop;
  for(loop = 0; loop < (int) tabList.count(); loop++)
  {
    QTabWidget *it = tabList.at(loop);
    if (it->isHidden())
      continue;
    QString s = it->label(it->currentPageIndex());
    Plot *p = plotList[s];
    if (p)
      p->draw();
  }
}

void QtstalkerApp::slotPaperTradeChanged (bool d)
{
  toolbar2->paperTradeClicked(d);
  slotChartUpdated();
}

void QtstalkerApp::slotWakeup ()
{
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

// remove



