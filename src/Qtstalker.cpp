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
#include <qstringlist.h>

#include "Qtstalker.h"
#include "DataWindow.h"
#include "ChartPage.h"
#include "PlotLine.h"
#include "PrefDialog.h"
#include "ChartDb.h"
#include "HelpWindow.h"

#include "qtstalker.xpm"

QtstalkerApp::QtstalkerApp()
{
  recordList = 0;
  quoteDialog = 0;
  status = None;
  plotList.setAutoDelete(TRUE);
  setIcon(qtstalker);
  currentMacro = 0;

  config.setup();

  initMenuBar();
  
  initToolBar();

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
  navTab = new NavigatorTab(dpSplitter);
  connect(navTab, SIGNAL(signalPositionChanged(int)), this, SLOT(slotNavigatorPosition(int)));
  connect(menubar, SIGNAL(signalFocusEvent(int)), navTab, SLOT(pressButton(int)));
  
  // setup the data panel area
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
  initPlot(mainPlot);
  mainPlot->setLogScale(menubar->getStatus(MainMenubar::Log));
  connect(menubar, SIGNAL(signalLog(bool)), mainPlot, SLOT(slotLogScaleChanged(bool)));
  connect(menubar, SIGNAL(signalHideMain(bool)), mainPlot, SLOT(slotHideMainChanged(bool)));

  tabs = new IndicatorTab(split);
  connect(tabs, SIGNAL(currentChanged(QWidget *)), this, SLOT(slotTabChanged(QWidget *)));

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

  QStringList l = config.getIndicators();
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
  
  navTab->togglePosition(navTab->getPosition());

  // setup the indicator page  
  ip->refreshList();
  
  // make sure the focus is set to the chart panel
  navTab->buttonPressed(0);
  
  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotQuit()));
  
  statusBar()->message(tr("Ready"), 2000);
}

QtstalkerApp::~QtstalkerApp()
{
}

void QtstalkerApp::initMenuBar()
{
  menubar = new MainMenubar(this);
  connect(menubar, SIGNAL(signalExit()), qApp, SLOT(quit()));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), menubar, SLOT(setKeyFlag(bool)));
  connect(menubar, SIGNAL(signalRunMacro(QString)), this, SLOT(slotRunMacro(QString)));
  connect(menubar, SIGNAL(signalSidePanel(bool)), this, SLOT(slotHideNav(bool)));
  connect(menubar, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  connect(menubar, SIGNAL(signalOptions()), this, SLOT(slotOptions()));
  connect(menubar, SIGNAL(signalQuotes()), this, SLOT(slotQuotes()));
}

void QtstalkerApp::initToolBar()
{
  // construct the chart toolbar
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

  toolbar2 = new ChartToolbar(this);//new
  connect(toolbar2, SIGNAL(signalCompressionChanged(int)), this, SLOT(slotCompressionChanged(int)));
  connect(toolbar2, SIGNAL(signalChartTypeChanged(int)), this, SLOT(slotChartTypeChanged(int)));
  connect(toolbar2, SIGNAL(signalPixelspaceChanged(int)), this, SLOT(slotPixelspaceChanged(int)));
  connect(menubar, SIGNAL(signalToolbarFocusEvent()), toolbar2, SLOT(setFocus()));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), toolbar2, SLOT(setKeyFlag(bool)));
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
    
  if (currentMacro)
    delete currentMacro;
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
    connect(quoteDialog, SIGNAL(chartUpdated()), this, SLOT(slotChartUpdated()));
    connect(quoteDialog, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
    connect(quoteDialog, SIGNAL(destroyed()), this, SLOT(slotExitQuoteDialog()));
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
    
    if (ip->getIndicatorStatus(selection))
    {
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
  else
  {
    QDictIterator<Plot> it(plotList);
    for(; it.current(); ++it)
    {
      if (! it.current()->getTabFlag())
        it.current()->draw();
    }
  }
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
  
  initPlot(plot);

  plot->setDateFlag(menubar->getStatus(MainMenubar::IndicatorDate));
  connect(menubar, SIGNAL(signalPlotDate(bool)), plot, SLOT(slotDateFlagChanged(bool)));

  if (tabFlag == Indicator::TabPlot)
  {
    tabs->addTab(plot, d);
    tabs->showPage(plot);
    tabs->adjustSize();
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
    plot->setPlotFont(QFont());

  plot->setGridFlag(menubar->getStatus(MainMenubar::Grid));
  plot->setScaleToScreen(menubar->getStatus(MainMenubar::ScaleToScreen));
  plot->setPixelspace(toolbar2->getPixelspace());
  plot->setIndex(toolbar2->getSlider());
  plot->setInterval((BarData::BarCompression) toolbar2->getCompressionInt());
  plot->setCrosshairsStatus(config.getData(Config::Crosshairs).toInt());  
  plot->setDrawMode(menubar->getStatus(MainMenubar::DrawMode));
    
  connect(plot, SIGNAL(signalNewIndicator()), this, SLOT(slotNewIndicator()));
  connect(plot, SIGNAL(signalEditIndicator(QString)), this, SLOT(slotEditIndicator(QString)));
  connect(plot, SIGNAL(statusMessage(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(plot, SIGNAL(infoMessage(Setting *)), this, SLOT(slotUpdateInfo(Setting *)));
  connect(plot, SIGNAL(leftMouseButton(int, int, bool)), this, SLOT(slotPlotLeftMouseButton(int, int, bool)));
  connect(plot, SIGNAL(signalMinPixelspace(int)), this, SLOT(slotMinPixelspaceChanged(int)));
  connect(plot, SIGNAL(signalCrosshairsStatus(bool)), this, SLOT(slotCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalCrosshairsStatus(bool)), plot, SLOT(setCrosshairsStatus(bool)));
  connect(this, SIGNAL(signalPixelspace(int)), plot, SLOT(setPixelspace(int)));
  connect(this, SIGNAL(signalIndex(int)), plot, SLOT(setIndex(int)));
  connect(this, SIGNAL(signalInterval(BarData::BarCompression)), plot, SLOT(setInterval(BarData::BarCompression)));
  connect(this, SIGNAL(signalChartPath(QString)), plot, SLOT(setChartPath(QString)));
  
  connect(toolbar2, SIGNAL(signalSliderChanged(int)), plot, SLOT(slotSliderChanged(int)));
  connect(menubar, SIGNAL(signalGrid(bool)), plot, SLOT(slotGridChanged(bool)));
  connect(menubar, SIGNAL(signalScale(bool)), plot, SLOT(slotScaleToScreenChanged(bool)));
  connect(menubar, SIGNAL(signalDraw(bool)), plot, SLOT(slotDrawModeChanged(bool)));
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
  tp = new TestPage(baseWidget);
  connect(tp, SIGNAL(message(QString)), this, SLOT(slotStatusMessage(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), tp, SLOT(setKeyFlag(bool)));
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
  connect(this, SIGNAL(signalSetKeyFlag(bool)), ip, SLOT(setKeyFlag(bool)));
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

void QtstalkerApp::slotToolbarFocus ()
{
  toolbar2->setFocus();
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

//***************************************************************
//*********** MACRO FUNCTIONS ***********************************
//***************************************************************

void QtstalkerApp::initMacroNav ()
{
  mp = new MacroPage(baseWidget);
  connect(mp, SIGNAL(signalRunMacro(QString)), this, SLOT(slotRunMacro(QString)));
  connect(mp, SIGNAL(signalRecordMacro(QString)), this, SLOT(slotRecordMacro(QString)));
  connect(this, SIGNAL(signalSetKeyFlag(bool)), mp, SLOT(setKeyFlag(bool)));
  navTab->addWidget(mp, 6);
}

void QtstalkerApp::slotRunMacro (QString d)
{
  slotStatusMessage(tr("Running macro..."));

  Macro *m = new Macro(d);
  m->load();
  
  int loop;
  for (loop = 0; loop < (int) m->getCount(); loop++)
  {
    m->setKey(loop);
    
    switch(m->getZone())
    {
      case Macro::ChartPage:
        chartNav->getNav()->doKeyPress(m->getKey());
	break;
      case Macro::GroupPage:
        gp->getNav()->doKeyPress(m->getKey());
	break;
      case Macro::IndicatorPage:
        ip->doKeyPress(m->getKey());
	break;
      case Macro::PortfolioPage:
        pp->doKeyPress(m->getKey());
	break;
      case Macro::TestPage:
        tp->doKeyPress(m->getKey());
	break;
      case Macro::ScannerPage:
        sp->doKeyPress(m->getKey());
	break;
      case Macro::MacroPage:
        mp->doKeyPress(m->getKey());
	break;
      case Macro::SidePanel:
        navTab->doKeyPress(m->getKey());
	break;
      case Macro::ChartToolbar:
//        toolbar2->doKeyPress(m->getKey());
	break;
      case Macro::Menubar:
        menubar->doKeyPress(m->getKey());
	break;
      default:
        break;
    }
  }
  
  delete m;
  
  slotStatusMessage(tr("Macro finished"));
}

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
  connect(chartNav->getNav(), SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(gp->getNav(), SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(pp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(tp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(sp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  connect(mp, SIGNAL(signalKeyPressed(int, int, int, int, QString)), currentMacro, SLOT(recordKey(int, int, int, int, QString)));
  
  emit signalSetKeyFlag(TRUE);
  
  currentMacro->record();
  
  slotStatusMessage(tr("Recording macro session..."));
}

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




