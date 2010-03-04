/*
 *  QtStalkerTester
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#include <QMessageBox>
#include <QMenuBar>
#include <QToolBar>
#include <QApplication>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QtDebug>
#include <QIcon>
#include <QSqlQuery>
#include <QWidget>

#include "../pics/done.xpm"
#include "../pics/newchart.xpm"
#include "../pics/open.xpm"
#include "../pics/save.xpm"
#include "../pics/about.xpm"
#include "../pics/script.xpm"

#include "QtStalkerTester.h"
#include "TestConfig.h"
#include "TestDataBase.h"
#include "QuoteDataBase.h"
#include "ExScript.h"
#include "Indicator.h"
#include "Setting.h"
#include "PluginFactory.h"

QtStalkerTester::QtStalkerTester ()
{
//  setWindowIcon(QIcon());
  QApplication::setOrganizationName("QtStalkerTester");
  setWindowTitle("QtStalkerTester: " +  tr("Empty"));

  // setup the databases, order is important
  TestConfig config;
  config.init(QString("0"));

  // get the plugin inventory
  PluginFactory fac;
  QString s;
  config.getData(TestConfig::IndicatorPluginPath, s);
  QStringList l;
  fac.getPluginList(s, l);
  config.setBaseData(TestConfig::IndicatorPluginList, l);
  config.getData(TestConfig::PlotPluginPath, s);
  fac.getPluginList(s, l);
  config.setBaseData(TestConfig::PlotPluginList, l);
  config.getData(TestConfig::COPluginPath, s);
  fac.getPluginList(s, l);
  config.setBaseData(TestConfig::COPluginList, l);

  TestDataBase tdb;
  tdb.init();

  QuoteDataBase qdb;
  QString dbFile;
  config.getData(TestConfig::DbName, dbFile);
  qdb.init(dbFile);

  createActions();
  createMenuBar();
  createToolBars();

  QWidget *w = new QWidget;
  setCentralWidget(w);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  w->setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  rankings = new TestRankings;
  rankings->update();
  tabs->addTab(rankings, tr("Rankings"));

  settings = new TestSettings;
  tabs->addTab(settings, tr("Settings"));

  report = new TestReport;
  tabs->addTab(report, tr("Report"));

//  chart = new TestChart;
//  tabs->addTab(chart, tr("Chart"));

  // restore app settings from previous session
  restoreSettings();

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

  statusbar = statusBar();
  statusbar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

void QtStalkerTester::createActions ()
{
  // create the actions needed for menu and toolbar

  exitAction = new QAction(QIcon(done_xpm), tr("E&xit"), this);
  exitAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  exitAction->setStatusTip(tr("Quit QtStalkerTester"));
  exitAction->setToolTip(tr("Quit QtStalkerTester"));
  connect(exitAction, SIGNAL(activated()), qApp, SLOT(quit()));

  newAction = new QAction(QIcon(newchart_xpm), tr("&New"), this);
  newAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  newAction->setStatusTip(tr("New Test"));
  newAction->setToolTip(tr("New Test"));
  connect(newAction, SIGNAL(activated()), this, SLOT(newTest()));

  openAction = new QAction(QIcon(open_xpm), tr("&Open"), this);
  openAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_O));
  openAction->setStatusTip(tr("Open Test"));
  openAction->setToolTip(tr("Open Test"));
  connect(openAction, SIGNAL(activated()), this, SLOT(openTest()));

  saveAction = new QAction(QIcon(save_xpm), tr("&Save"), this);
  saveAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  saveAction->setStatusTip(tr("Save Test"));
  saveAction->setToolTip(tr("Save Test"));
  connect(saveAction, SIGNAL(activated()), this, SLOT(saveTest()));

  runAction = new QAction(QIcon(script_xpm), tr("&Run"), this);
  runAction->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  runAction->setStatusTip(tr("Run Test"));
  runAction->setToolTip(tr("Run Test"));
  connect(runAction, SIGNAL(activated()), this, SLOT(run()));

  aboutAction = new QAction(QIcon(about_xpm), tr("&About"), this);
  aboutAction->setStatusTip(tr("About QtStalkerTester"));
  aboutAction->setToolTip(tr("About QtStalkerTester"));
  connect(aboutAction, SIGNAL(activated()), this, SLOT(about()));
}

void QtStalkerTester::createMenuBar ()
{
  // create the main menubar
  QMenuBar *menubar = menuBar();

  QMenu *menu = new QMenu;
  menu->setTitle(tr("&File"));
  menu->addAction(newAction);
  menu->addAction(openAction);
  menu->addAction(saveAction);
  menu->addAction(runAction);
  menu->addSeparator();
  menu->addAction(exitAction);
  menubar->addMenu(menu);

  menu = new QMenu;
  menu->setTitle(tr("&Help"));
  menu->addAction(aboutAction);
  menubar->addMenu(menu);
}

void QtStalkerTester::createToolBars ()
{
  // construct the button toolbar
  QToolBar *toolbar = addToolBar("buttonToolbar");

  toolbar->addAction(exitAction);
  toolbar->addAction(newAction);
  toolbar->addAction(openAction);
  toolbar->addAction(saveAction);
  toolbar->addAction(runAction);
  toolbar->addAction(aboutAction);
}

void QtStalkerTester::quit ()
{
  TestConfig config;
  config.transaction();

  // save app size
  QSize sz = size();
  config.setBaseData((int) TestConfig::MainWindowSize, sz);

  // save app position
  QPoint pt = pos();
  config.setBaseData((int) TestConfig::MainWindowPos, pt);

  config.commit();
}

void QtStalkerTester::restoreSettings ()
{
  TestConfig config;

  // restore the size of the app
  QSize sz;
  config.getBaseData((int) TestConfig::MainWindowSize, sz);
  resize(sz);

  // restore the position of the app
  QPoint p;
  config.getBaseData((int) TestConfig::MainWindowPos, p);
  move(p);
}

void QtStalkerTester::closeEvent (QCloseEvent *)
{
}

void QtStalkerTester::about ()
{
  // display the about dialog
  QString versionString = "QtStalkerTester\nVersion 0.1-dev (working title)\nBuilt using Qt ";
  versionString += QT_VERSION_STR;
  versionString += "\n(C) 2010 by Stefan Stratigakos\nqtstalker.sourceforge.net";
  versionString += "\nQtStalkerTester is licensed with GNU General Public License (GPL) version 2";
  QMessageBox::about(this, tr("About QtStalkerTester"), versionString);
}

void QtStalkerTester::saveTest ()
{
  if (name.isEmpty())
    return;

  QString s = "INSERT OR REPLACE INTO results  VALUES (";
  s.append("'" + name + "'");

  QString ts;
  settings->getScript(ts);
  if (ts.isEmpty())
    s.append(",' '");
  else
    s.append(",'" + ts + "'");

  s.append("," + QString::number(settings->getScriptCheck()));

  settings->getShellCommand(ts);
  if (ts.isEmpty())
    s.append(",' '");
  else
    s.append(",'" + ts + "'");

  s.append("," + QString::number(settings->getLongCheck()));
  s.append("," + QString::number(settings->getShortCheck()));

  settings->getSymbol(ts);
  if (ts.isEmpty())
    s.append(",' '");
  else
    s.append(",'" + ts + "'");

  s.append("," + QString::number(settings->getEnterField()));
  s.append("," + QString::number(settings->getExitField()));
  s.append("," + QString::number(settings->getBars()));
  s.append("," + QString::number(settings->getBarLength()));
  s.append("," + QString::number(settings->getEntryComm()));
  s.append("," + QString::number(settings->getExitComm()));
  s.append("," + QString::number(settings->getDelay()));
  s.append("," + QString::number(settings->getAccount()));
  s.append("," + QString::number(settings->getVolumePercentage()));
  s.append("," + QString::number(settings->getTrailing()));
  s.append("," + QString::number(settings->getTrailingCheck()));

  settings->getEnterLongCombo(ts);
  s.append(",'" + ts + "'");

  settings->getExitLongCombo(ts);
  s.append(",'" + ts + "'");

  settings->getEnterShortCombo(ts);
  s.append(",'" + ts + "'");

  settings->getExitShortCombo(ts);
  s.append(",'" + ts + "'");

  settings->getEnterLongSettings(ts);
  s.append(",'" + ts + "'");

  settings->getExitLongSettings(ts);
  s.append(",'" + ts + "'");

  settings->getEnterShortSettings(ts);
  s.append(",'" + ts + "'");

  settings->getExitShortSettings(ts);
  s.append(",'" + ts + "'");

  // summary stuff
  report->getGrossProfit(ts);
  s.append("," + ts);

  report->getNetProfit(ts);
  s.append("," + ts);

  report->getMaxDrawDown(ts);
  s.append("," + ts);

  report->getAvgDrawDown(ts);
  s.append("," + ts);

  report->getCommissions(ts);
  s.append("," + ts);

  report->getWinLossRatio(ts);
  s.append("," + ts);

  report->getTotalTrades(ts);
  s.append("," + ts);

  report->getPercentProfitable(ts);
  s.append("," + ts);

  report->getWinningTrades(ts);
  s.append("," + ts);

  report->getLosingTrades(ts);
  s.append("," + ts);

  report->getMaxWinTrade(ts);
  s.append("," + ts);

  report->getMaxLossTrade(ts);
  s.append("," + ts);

  report->getAvgWinTrade(ts);
  s.append("," + ts);

  report->getAvgLossTrade(ts);
  s.append("," + ts);

  report->getMaxWinLong(ts);
  s.append("," + ts);

  report->getMaxLossLong(ts);
  s.append("," + ts);

  report->getMaxWinShort(ts);
  s.append("," + ts);

  report->getMaxLossShort(ts);
  s.append("," + ts);

  report->getTradeLog(ts);
  s.append(",'" + ts + "'");

  report->getBalance(ts);
  s.append("," + ts);

  s.append(")");

  TestDataBase db;
  db.setTest(s);
}

void QtStalkerTester::newTest ()
{
  bool ok;
  QString s = QInputDialog::getText(this,
                                    tr("New Test"),
                                    tr("Enter name of new test."),
                                    QLineEdit::Normal,
                                    QString(),
                                    &ok,
                                    0);
  if (! ok || s.isEmpty())
    return;

  // check is name already exists
  QStringList l;
  TestDataBase db;
  db.getTests(l);
  if (l.contains(s))
  {
    QMessageBox::information(this, tr("QtStalkerTester: Error"), tr("This test already exists."));
    return;
  }

  loadTest(s);
}

void QtStalkerTester::openTest ()
{
  QStringList l;
  TestDataBase db;
  db.getTests(l);

  bool ok;
  QString test = QInputDialog::getItem(this,
					QString(tr("Open Test")),
					QString(tr("Select test to open")),
                                        l,
					0,
					FALSE,
                                        &ok,
					0);

  if (test.isEmpty())
    return;

  loadTest(test);
}

void QtStalkerTester::loadTest (QString &s)
{
  name = s;

  TestDataBase db;
  QSqlQuery q;
  db.getTest(s, q);

  setWindowTitle("QtStalkerTester: " + s);

  while (q.next())
  {
    int col = 1;
    QString ts = q.value(col++).toString();
    settings->setScript(ts);

    settings->setScriptCheck(q.value(col++).toBool());

    ts = q.value(col++).toString();
    settings->setShellCommand(ts);

    settings->setLongCheck(q.value(col++).toBool());

    settings->setShortCheck(q.value(col++).toBool());

    ts = q.value(col++).toString();
    settings->setSymbol(ts);

    settings->setEnterField(q.value(col++).toInt());

    settings->setExitField(q.value(col++).toInt());

    settings->setBars(q.value(col++).toInt());

    settings->setBarLength(q.value(col++).toInt());

    settings->setEntryComm(q.value(col++).toDouble());

    settings->setExitComm(q.value(col++).toDouble());

    settings->setDelay(q.value(col++).toInt());

    settings->setAccount(q.value(col++).toDouble());

    settings->setVolumePercentage(q.value(col++).toDouble());

    settings->setTrailing(q.value(col++).toDouble());

    settings->setTrailingCheck(q.value(col++).toBool());

    ts = q.value(col++).toString();
    settings->setEnterLongCombo(ts);

    ts = q.value(col++).toString();
    settings->setExitLongCombo(ts);

    ts = q.value(col++).toString();
    settings->setEnterShortCombo(ts);

    ts = q.value(col++).toString();
    settings->setExitShortCombo(ts);

    ts = q.value(col++).toString();
    settings->setEnterLongSettings(ts);

    ts = q.value(col++).toString();
    settings->setExitLongSettings(ts);

    ts = q.value(col++).toString();
    settings->setEnterShortSettings(ts);

    ts = q.value(col++).toString();
    settings->setExitShortSettings(ts);

    // summary stuff
    ts = q.value(col++).toString();
    report->setGrossProfit(ts);

    ts = q.value(col++).toString();
    report->setNetProfit(ts);

    ts = q.value(col++).toString();
    report->setMaxDrawDown(ts);

    ts = q.value(col++).toString();
    report->setAvgDrawDown(ts);

    ts = q.value(col++).toString();
    report->setCommissions(ts);

    ts = q.value(col++).toString();
    report->setWinLossRatio(ts);

    ts = q.value(col++).toString();
    report->setTotalTrades(ts);

    ts = q.value(col++).toString();
    report->setPercentProfitable(ts);

    ts = q.value(col++).toString();
    report->setWinningTrades(ts);

    ts = q.value(col++).toString();
    report->setLosingTrades(ts);

    ts = q.value(col++).toString();
    report->setMaxWinTrade(ts);

    ts = q.value(col++).toString();
    report->setMaxLossTrade(ts);

    ts = q.value(col++).toString();
    report->setAvgWinTrade(ts);

    ts = q.value(col++).toString();
    report->setAvgLossTrade(ts);

    ts = q.value(col++).toString();
    report->setMaxWinLong(ts);

    ts = q.value(col++).toString();
    report->setMaxLossLong(ts);

    ts = q.value(col++).toString();
    report->setMaxWinShort(ts);

    ts = q.value(col++).toString();
    report->setMaxLossShort(ts);

    ts = q.value(col++).toString();
    report->setTradeLog(ts);

    ts = q.value(col++).toString();
    report->setBalance(ts);
  }
}

void QtStalkerTester::cancelTest ()
{
}

void QtStalkerTester::run ()
{
  if (settings->getScriptCheck())
  {
    QString s;
    settings->getScript(s);
    if (s.isEmpty())
    {
      QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing script file."));
      return;
    }

    settings->getShellCommand(s);
    if (s.isEmpty())
    {
      QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing shell command."));
      return;
    }
  }

  QString s;
  settings->getSymbol(s);
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing symbol."));
    return;
  }

  BarData *data = new BarData;
  data->setSymbol(s);
  data->setBarLength((BarData::BarLength) settings->getBarLength());
  data->setBarsRequested(settings->getBars());

  QuoteDataBase qdb;
  qdb.getChart(data);
  if (data->count() == 0)
  {
    qDebug() << "QtStalkerTester::run: 0 bars loaded";
    delete data;
    return;
  }

  TestSignal enterLongSigs;
  TestSignal exitLongSigs;
  TestSignal enterShortSigs;
  TestSignal exitShortSigs;
  
  TestConfig config;
  QString path;
  config.getData(TestConfig::IndicatorPluginPath, path);

  PluginFactory fac;

  if (settings->getScriptCheck())
  {
    ExScript server(path);
    server.setBarData(data);
    server.setDeleteFlag(TRUE);
    settings->getShellCommand(s);
    QString ts;
    settings->getScript(ts);
    s.append(" " + ts);
    server.calculate(s); // we are calling the blocking version of calculate() here

    PlotLine *l = server.getEnterLong();
    if (l)
      enterLongSigs.createSignals(data, l);

    l = server.getExitLong();
    if (l)
      exitLongSigs.createSignals(data, l);

    l = server.getEnterShort();
    if (l)
      enterShortSigs.createSignals(data, l);

    l = server.getExitShort();
    if (l)
      exitShortSigs.createSignals(data, l);
  }
  else
  {
    if (settings->getLongCheck())
    {
      // enter long indicator
      Indicator i;
      settings->getEnterLongIndicator(i);
      QString s;
      i.getIndicator(s);
      IndicatorPlugin *plug = fac.getIndicator(path, s);
      if (! plug)
      {
	delete data;
        return;
      }
      
      plug->setSettings(i);
      if (plug->test(data, enterLongSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: enterLong error";
        return;
      }

      // exit long indicator
      i.deleteAll();
      settings->getExitLongIndicator(i);
      i.getIndicator(s);
      plug = fac.getIndicator(path, s);
      if (! plug)
      {
	delete data;
        return;
      }
      
      plug->setSettings(i);
      if (plug->test(data, exitLongSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: exitLong error";
        return;
      }
    }

    if (settings->getShortCheck())
    {
      // enter short indicator
      Indicator i;
      settings->getEnterShortIndicator(i);
      QString s;
      i.getIndicator(s);
      IndicatorPlugin *plug = fac.getIndicator(path, s);
      if (! plug)
      {
	delete data;
        return;
      }
      
      plug->setSettings(i);
      if (plug->test(data, enterShortSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: enterShort error";
        return;
      }

      // exit short indicator
      i.deleteAll();
      settings->getExitShortIndicator(i);
      i.getIndicator(s);
      plug = fac.getIndicator(path, s);
      if (! plug)
      {
	delete data;
        return;
      }
      
      plug->setSettings(i);
      if (plug->test(data, exitShortSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: exitShort error";
        return;
      }
    }
  }

  QList<TestTrade *> trades;
  TestTrade ttrade;
  if (settings->getScriptCheck())
  {
    ttrade.createTrades(data, trades, 0, settings->getVolumePercentage(), settings->getDelay(),
		        settings->getEnterField(), enterLongSigs, exitLongSigs);
    ttrade.createTrades(data, trades, 1, settings->getVolumePercentage(), settings->getDelay(),
	                settings->getExitField(), enterShortSigs, exitShortSigs);
  }
  else
  {
    if (settings->getLongCheck())
    {
      ttrade.createTrades(data, trades, 0, settings->getVolumePercentage(), settings->getDelay(),
		          settings->getEnterField(), enterLongSigs, exitLongSigs);
    }

    if (settings->getShortCheck())
    {
      ttrade.createTrades(data, trades, 1, settings->getVolumePercentage(), settings->getDelay(),
	                  settings->getExitField(), enterShortSigs, exitShortSigs);
    }
  }

  runTrades(data, trades);

  report->createSummary(trades, settings->getAccount(), settings->getEntryComm(), settings->getExitComm());

  saveTest();

  rankings->update();
  
//  chart->update(data, trades);

  delete data;
  qDeleteAll(trades);
}

void QtStalkerTester::runTrades (BarData *data, QList<TestTrade *> &trades)
{
  PlotLine *line = 0;
  switch (settings->getExitField())
  {
    case 0:
      line = data->getInput((BarData::Open));
      break;
    case 1:
      line = data->getInput((BarData::High));
      break;
    case 2:
      line = data->getInput((BarData::Low));
      break;
    case 4:
      line = data->getInput((BarData::AveragePrice));
      break;
    case 5:
      line = data->getInput((BarData::MedianPrice));
      break;
    case 6:
      line = data->getInput((BarData::TypicalPrice));
      break;
    default:
      line = data->getInput((BarData::Close));
      break;
  }
  if (! line)
    return;

  int loop;
  double money = settings->getAccount();
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    if (settings->getTrailingCheck())
      trade->setTrailing(settings->getTrailing());
    trade->update(line, data, money);
    money = money + trade->getProfit();
  }

  delete line;
}

