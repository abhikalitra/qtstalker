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
#include <QVBoxLayout>
#include <QApplication>
#include <QInputDialog>
#include <QFileDialog>
#include <QDir>
#include <QtDebug>
#include <QIcon>

#include "../pics/done.xpm"
#include "../pics/newchart.xpm"
#include "../pics/open.xpm"
#include "../pics/save.xpm"
#include "../pics/about.xpm"

#include "QtStalkerTester.h"
#include "TestConfig.h"
#include "TestDataBase.h"
#include "BarData.h"
#include "ExScript.h"
#include "QuoteDataBase.h"

QtStalkerTester::QtStalkerTester ()
{
  priceList << "Open" << "High" << "Low" << "Close" << "AvgPrice" << "MedianPrice" << "TypicalPrice";

  BarData bd;
  bd.getBarLengthList(barLengthList);

//  setWindowIcon(QIcon());
  QApplication::setOrganizationName("QtStalkerTester");

  // setup the databases, order is important
  TestConfig config;
  config.init(QString("0"));

  TestDataBase tdb;
  tdb.init();

  QuoteDataBase qdb;
  QString plugin, dbFile, host, user, pass;
  config.getData(TestConfig::DbPlugin, plugin);
  config.getData(TestConfig::DbName, dbFile);
  config.getData(TestConfig::DbHostName, host);
  config.getData(TestConfig::DbUserName, user);
  config.getData(TestConfig::DbPassword, pass);
  qdb.init(plugin, dbFile, host, user, pass);

  createActions();
  createMenuBar();
  createToolBars();

  baseWidget = new QWidget;
  setCentralWidget(baseWidget);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  baseWidget->setLayout(vbox);

  tabs = new QTabWidget;
  vbox->addWidget(tabs);

  buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(run()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(cancelTest()));
//  connect(buttonBox, SIGNAL(helpRequested()), this, SLOT(help()));
  vbox->addWidget(buttonBox);

  createRuleTab();
  createStopsTab();
  createReportTab();

  // restore app settings from previous session
  restoreSettings();

  // catch any kill signals and try to save config
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(quit()));

  statusbar = statusBar();
  statusbar->showMessage(tr("Ready"), 2000);

  setUnifiedTitleAndToolBarOnMac(TRUE);
}

QtStalkerTester::~QtStalkerTester ()
{
}

void QtStalkerTester::createRuleTab()
{
  QWidget *w = new QWidget(this);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // script parms
  QLabel *label = new QLabel(tr("Script"));
  grid->addWidget(label, row, col++);

//  scriptButton = new QPushButton;
//  grid->addWidget(scriptButton, row++, col--);
//  connect(scriptButton, SIGNAL(pressed()), this, SLOT(scriptButtonPressed()));
  script = new QLineEdit;
  grid->addWidget(script, row++, col--);

  // symbol parms
  label = new QLabel(tr("Symbol"));
  grid->addWidget(label, row, col++);

//  QPushButton *pb = new QPushButton(QString::number(0) + tr(" Symbols"));
//  pb->setToolTip(tr("Select symbols for test"));
//  connect(pb, SIGNAL(pressed()), this, SLOT(symbolButtonPressed()));
  symbol = new QLineEdit;
  grid->addWidget(symbol, row++, col--);

  // enter field parms
  label = new QLabel(tr("Enter Price"));
  grid->addWidget(label, row, col++);

  enterField = new QComboBox;
  enterField->addItems(priceList);
  grid->addWidget(enterField, row++, col--);

  // exit field parms
  label = new QLabel(tr("Exit Price"));
  grid->addWidget(label, row, col++);

  exitField = new QComboBox;
  exitField->addItems(priceList);
  grid->addWidget(exitField, row++, col--);

  // bar length parm
  label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, row, col++);

  barLength = new QComboBox;
  barLength->addItems(barLengthList);
  grid->addWidget(barLength, row++, col--);

  // bars
  label = new QLabel(tr("Bars"));
  grid->addWidget(label, row, col++);

  bars = new QSpinBox;
  bars->setMinimum(0);
  bars->setMaximum(999999);
  bars->setValue(275);
  grid->addWidget(bars, row++, col--);

  // start a new parm section on the right side of the page
  grid->setRowStretch(row, 1);
  row = 0;
  col = 2;

  // entry comm
  label = new QLabel(tr("Entry Commission"));
  grid->addWidget(label, row, col++);

  entryComm = new QDoubleSpinBox;
  entryComm->setMinimum(0);
  entryComm->setMaximum(999999);
  entryComm->setValue(10);
  grid->addWidget(entryComm, row++, col--);

  // exit comm
  label = new QLabel(tr("Exit Commission"));
  grid->addWidget(label, row, col++);

  exitComm = new QDoubleSpinBox;
  exitComm->setMinimum(0);
  exitComm->setMaximum(999999);
  exitComm->setValue(10);
  grid->addWidget(exitComm, row++, col--);

//  grid->setRowStretch(row, 1);

  tabs->addTab(w, tr("Rule"));
}

void QtStalkerTester::createStopsTab()
{
  QWidget *w = new QWidget(this);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // trailing stop parms
  trailingCheck = new QCheckBox(tr("Trailing"));
  grid->addWidget(trailingCheck, row, col++);

  trailing = new QDoubleSpinBox;
  trailing->setMinimum(0);
  trailing->setMaximum(100);
  trailing->setValue(7);
  grid->addWidget(trailing, row++, col--);

  grid->setRowStretch(row, 1);

  tabs->addTab(w, tr("Stops"));
}

void QtStalkerTester::createReportTab()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // gross profit
  QLabel *label = new QLabel(tr("Gross Profit"));
  grid->addWidget(label, row, col++);

  grossProfit = new QLabel;
  grid->addWidget(grossProfit, row++, col--);

  // net profit
  label = new QLabel(tr("Net Profit"));
  grid->addWidget(label, row, col++);

  netProfit = new QLabel;
  grid->addWidget(netProfit, row++, col--);

  // max drawdown
  label = new QLabel(tr("Maximum Drawdown"));
  grid->addWidget(label, row, col++);

  maxDrawDown = new QLabel;
  grid->addWidget(maxDrawDown, row++, col--);

  // avg drawdown
  label = new QLabel(tr("Average Drawdown"));
  grid->addWidget(label, row, col++);

  avgDrawDown = new QLabel;
  grid->addWidget(avgDrawDown, row++, col--);

  // commissions
  label = new QLabel(tr("Commissions"));
  grid->addWidget(label, row, col++);

  commissions = new QLabel;
  grid->addWidget(commissions, row++, col--);

  // win/loss ratio
  label = new QLabel(tr("Win/Loss Ratio"));
  grid->addWidget(label, row, col++);

  winLossRatio = new QLabel;
  grid->addWidget(winLossRatio, row++, col--);

  // total trades
  label = new QLabel(tr("Total Trades"));
  grid->addWidget(label, row, col++);

  totalTrades = new QLabel;
  grid->addWidget(totalTrades, row++, col--);

  // percent profitable
  label = new QLabel(tr("Percent Profitable"));
  grid->addWidget(label, row, col++);

  percentProfitable = new QLabel;
  grid->addWidget(percentProfitable, row++, col--);

  // winning trades
  label = new QLabel(tr("Winning Trades"));
  grid->addWidget(label, row, col++);

  winningTrades = new QLabel;
  grid->addWidget(winningTrades, row++, col--);

  // losing trades
  label = new QLabel(tr("Losing Trades"));
  grid->addWidget(label, row, col++);

  losingTrades = new QLabel;
  grid->addWidget(losingTrades, row++, col--);

  col = 2;
  row = 0;

  // maxWinTrade
  label = new QLabel(tr("Largest Winning Trade"));
  grid->addWidget(label, row, col++);

  maxWinTrade = new QLabel;
  grid->addWidget(maxWinTrade, row++, col--);

  // maxLosingTrade
  label = new QLabel(tr("Largest Losing Trade"));
  grid->addWidget(label, row, col++);

  maxLossTrade = new QLabel;
  grid->addWidget(maxLossTrade, row++, col--);

  // avgWinTrade
  label = new QLabel(tr("Average Winning Trade"));
  grid->addWidget(label, row, col++);

  avgWinTrade = new QLabel;
  grid->addWidget(avgWinTrade, row++, col--);

  // avgLosingTrade
  label = new QLabel(tr("Average Losing Trade"));
  grid->addWidget(label, row, col++);

  avgLossTrade = new QLabel;
  grid->addWidget(avgLossTrade, row++, col--);

  // maxWinLong
  label = new QLabel(tr("Largest Winning Long"));
  grid->addWidget(label, row, col++);

  maxWinLong = new QLabel;
  grid->addWidget(maxWinLong, row++, col--);

  // maxLosingLong
  label = new QLabel(tr("Largest Losing Long"));
  grid->addWidget(label, row, col++);

  maxLossLong = new QLabel;
  grid->addWidget(maxLossLong, row++, col--);

  // maxWinShort
  label = new QLabel(tr("Largest Winning Short"));
  grid->addWidget(label, row, col++);

  maxWinShort = new QLabel;
  grid->addWidget(maxWinShort, row++, col--);

  // maxLosingShort
  label = new QLabel(tr("Largest Losing Short"));
  grid->addWidget(label, row, col++);

  maxLossShort = new QLabel;
  grid->addWidget(maxLossShort, row++, col--);

  tradeLog = new QTextEdit;
  tradeLog->setReadOnly(TRUE);
  vbox->addWidget(tradeLog);

  tabs->addTab(w, tr("Report"));
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
  newAction->setStatusTip(tr("New Equipment Database"));
  newAction->setToolTip(tr("New Equipment Database"));
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
  getSettings();

  TestDataBase db;
  db.setTest(test);
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

void QtStalkerTester::scriptButtonPressed ()
{
  QString dir;
  TestConfig config;
  config.getData(TestConfig::ScriptDirectory, dir);
  if (dir.isEmpty())
    dir = QDir::homePath();

  scriptFile = QFileDialog::getOpenFileName(this,
					    QString(tr("Select Test Script")),
					    dir,
					    QString(),
					    0,
					    0);
  if (scriptFile.isEmpty())
    return;

  QFileInfo fi(scriptFile);
  QString s = fi.absolutePath();
  config.setData(TestConfig::ScriptDirectory, s);

  scriptButton->setText(fi.completeBaseName());
}

void QtStalkerTester::symbolButtonPressed ()
{
  // custom symbol selection dialog here
}

void QtStalkerTester::loadTest (QString &s)
{
  TestDataBase db;
  test.setName(s);
  db.getTest(test);

  QString ts;
  test.getScript(ts);
//  QFileInfo fi(scriptFile);
//  scriptButton->setText(fi.completeBaseName());
  script->setText(ts);

  test.getSymbol(ts);
  symbol->setText(ts);

  test.getEnterField(ts);
  int index = priceList.indexOf(ts);
  enterField->setCurrentIndex(index);

  test.getExitField(ts);
  index = priceList.indexOf(ts);
  exitField->setCurrentIndex(index);

  barLength->setCurrentIndex(test.getBarLength());

  bars->setValue(test.getBars());

  entryComm->setValue(test.getEntryComm());

  exitComm->setValue(test.getExitComm());

  // summary items
  grossProfit->setNum(test.getGrossProfit());
  netProfit->setNum(test.getNetProfit());
  maxDrawDown->setNum(test.getMaxDrawDown());
  avgDrawDown->setNum(test.getAvgDrawDown());
  commissions->setNum(test.getTotalComm());
  winLossRatio->setNum(test.getWinLossRatio());
  totalTrades->setNum(test.getTrades());
  percentProfitable->setNum(test.getProfitable());
  winningTrades->setNum(test.getWinTrades());
  losingTrades->setNum(test.getLossTrades());
  maxWinTrade->setNum(test.getMaxWinTrade());
  maxLossTrade->setNum(test.getMaxLossTrade());
  avgWinTrade->setNum(test.getAvgWinTrade());
  avgLossTrade->setNum(test.getAvgLossTrade());
  maxWinLong->setNum(test.getMaxWinLong());
  maxLossLong->setNum(test.getMaxLossLong());
  maxWinShort->setNum(test.getMaxWinShort());
  maxLossShort->setNum(test.getMaxLossShort());
}

void QtStalkerTester::getSettings ()
{
  QString ts;
//  test.setScript(scriptFile);
  ts = script->text();
  test.setScript(ts);

  ts = symbol->text();
  test.setSymbol(ts);

  ts = enterField->currentText();
  test.setEnterField(ts);

  ts = exitField->currentText();
  test.setExitField(ts);

  test.setBarLength(barLength->currentIndex());

  test.setBars(bars->value());

  test.setEntryComm(entryComm->value());

  test.setExitComm(exitComm->value());

  test.setBarLength(barLength->currentIndex());

  test.setBars(bars->value());

  test.setEntryComm(entryComm->value());

  test.setExitComm(exitComm->value());
}

void QtStalkerTester::cancelTest ()
{
}

void QtStalkerTester::run ()
{
  getSettings();

  QString s;
  test.getScript(s);
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Invalid script command."));
    return;
  }

  test.getSymbol(s);
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Invalid symbol setting."));
    return;
  }

  BarData *data = new BarData;
  s = symbol->text();
  data->setSymbol(s);
  data->setBarLength((BarData::BarLength) barLength->currentIndex());
  data->setBarsRequested(bars->value());

  QuoteDataBase qdb;
  TestConfig config;
  QString sql, sqlfd, sqlld;
  config.getData(TestConfig::DbGetSymbol, sql);
  config.getData(TestConfig::DbFirstDate, sqlfd);
  config.getData(TestConfig::DbLastDate, sqlld);
  qdb.getChart(sql, sqlfd, sqlld, data);

  ExScript server;
  server.setBarData(data);
  test.getScript(s);
  server.calculate(s); // we are calling the blocking version of calculate() here

  QHash<int, TestSignal *> enterLongSigs;
  QList<PlotLine *> l;
  server.getEnterLongList(l);
  createSignals(data, l, enterLongSigs);

  l.clear();
  QHash<int, TestSignal *> exitLongSigs;
  server.getExitLongList(l);
  createSignals(data, l, exitLongSigs);

  l.clear();
  QHash<int, TestSignal *> enterShortSigs;
  server.getEnterShortList(l);
  createSignals(data, l, enterShortSigs);

  l.clear();
  QHash<int, TestSignal *> exitShortSigs;
  server.getExitShortList(l);
  createSignals(data,l, exitShortSigs);

  int loop;
  int status = 0;
  QList<TestTrade *> trades;
  for (loop = 0; loop < data->count(); loop++)
  {
    switch (status)
    {
      case 1: // currently long, check if we exit trade
      {
	TestSignal *sig = exitLongSigs.value(loop);
	if (sig)
	{
	  status = 0; // exit long
	  TestTrade *trade = trades.at(trades.count() - 1);
	  endTrade(loop, data, trade);
	}
	else
	{
	  TestTrade *trade = trades.at(trades.count() - 1);
	  currentTrade(loop, data, trade);
	}
	break;
      }
      case 2: // currently short, check if we exit trade
      {
	TestSignal *sig = exitShortSigs.value(loop);
	if (sig)
	{
	  status = 0; // exit short
	  TestTrade *trade = trades.at(trades.count() - 1);
	  endTrade(loop, data, trade);
	}
	else
	{
	  TestTrade *trade = trades.at(trades.count() - 1);
	  currentTrade(loop, data, trade);
	}
	break;
      }
      default:
      {
	TestSignal *sig = enterLongSigs.value(loop);
	if (sig)
	{
	  status = 1; // we are long
	  TestTrade *trade = new TestTrade;
	  trade->setType(0);
	  startTrade(loop, data, trade);
	  trades.append(trade);
	}

	sig = enterShortSigs.value(loop);
	if (sig)
	{
	  status = 2; // we are short
	  TestTrade *trade = new TestTrade;
	  trade->setType(1);
	  startTrade(loop, data, trade);
	  trades.append(trade);
	}

	break;
      }
    }
  }

  runTrades(data, trades);

  createSummary(trades);

  delete data;
  qDeleteAll(trades);
  qDeleteAll(enterLongSigs);
  qDeleteAll(exitLongSigs);
  qDeleteAll(enterShortSigs);
  qDeleteAll(exitShortSigs);
}

void QtStalkerTester::createSignals (BarData *data, QList<PlotLine *> &l, QHash<int, TestSignal*> &sigs)
{
  int loop;
  for (loop = 0; loop < l.count(); loop++)
  {
    PlotLine *line = l.at(loop);
    int barLoop = data->count() - line->getSize();
    if (barLoop < 0)
    {
      qDebug() << "QtStalkerTester::createSignals" << line->getSize() << "size > bars size";
      continue;
    }

    int status = 0;
    int lineLoop;
    for (lineLoop = 0; lineLoop < line->getSize(); lineLoop++, barLoop++)
    {
      switch (status)
      {
        case 1: // we are inside a signal
          if (line->getData(lineLoop) == 0)
 	    status = 0;
	  break;
        default: // we are outside a signal
          if (line->getData(lineLoop) == 1)
	  {
	    status = 1;
	    TestSignal *sig = sigs.value(barLoop);
	    if (! sig)
	    {
	      sig = new TestSignal;
	      sig->setBar(barLoop);
	      sigs.insert(barLoop, sig);
	      sig->addSignal();
	    }
	  }
	  break;
      }
    }
  }
}

void QtStalkerTester::startTrade (int pos, BarData *data, TestTrade *trade)
{
  QDateTime dt;
  data->getDate(pos, dt);
  trade->setEnterDate(dt, pos);

  int i = enterField->currentIndex();

  switch (i)
  {
    case 0:
      trade->setEnterPrice(data->getOpen(pos));
      break;
    case 1:
      trade->setEnterPrice(data->getHigh(pos));
      break;
    case 2:
      trade->setEnterPrice(data->getLow(pos));
      break;
    case 4:
      trade->setEnterPrice(data->getAvgPrice(pos));
      break;
    case 5:
      trade->setEnterPrice(data->getMedianPrice(pos));
      break;
    case 6:
      trade->setEnterPrice(data->getTypicalPrice(pos));
      break;
    default:
      trade->setEnterPrice(data->getClose(pos));
      break;
  }
}

void QtStalkerTester::currentTrade (int pos, BarData *data, TestTrade *trade)
{
  QDateTime dt;
  data->getDate(pos, dt);
  trade->setExitDate(dt, pos);
  trade->setExitPrice(data->getClose(pos));
}

void QtStalkerTester::endTrade (int pos, BarData *data, TestTrade *trade)
{
  QDateTime dt;
  data->getDate(pos, dt);
  trade->setExitDate(dt, pos);

  int i = exitField->currentIndex();

  switch (i)
  {
    case 0:
      trade->setExitPrice(data->getOpen(pos));
      break;
    case 1:
      trade->setExitPrice(data->getHigh(pos));
      break;
    case 2:
      trade->setExitPrice(data->getLow(pos));
      break;
    case 4:
      trade->setExitPrice(data->getAvgPrice(pos));
      break;
    case 5:
      trade->setExitPrice(data->getMedianPrice(pos));
      break;
    case 6:
      trade->setExitPrice(data->getTypicalPrice(pos));
      break;
    default:
      trade->setExitPrice(data->getClose(pos));
      break;
  }
}

void QtStalkerTester::createSummary (QList<TestTrade *> &trades)
{
  double totalDraw = 0;
  int longTrades = 0;
  int shortTrades = 0;
  double totalLossTrades = 0;
  int numDrawDown = 0;
  int loop;
  QString s;
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);

    if (trade->getDrawDown() < 0)
    {
      totalDraw = totalDraw + trade->getDrawDown();
      numDrawDown++;
    }

    if (trade->getDrawDown() < test.getMaxDrawDown())
      test.setMaxDrawDown(trade->getDrawDown());

    if (trade->getType() == 0)
      longTrades++;
    else
      shortTrades++;

    if (trade->getProfit() >= 0)
    {
      test.setWinTrades(test.getWinTrades() + 1);
      test.setGrossProfit(test.getGrossProfit() + trade->getProfit());
      if (trade->getProfit() > test.getMaxWinTrade())
      {
	test.setMaxWinTrade(trade->getProfit());
	if (trade->getType() == 0)
	  test.setMaxWinLong(test.getMaxWinTrade());
	else
	  test.setMaxWinShort(test.getMaxWinTrade());
      }
    }
    else
    {
      test.setLossTrades(test.getLossTrades() + 1);
      totalLossTrades = totalLossTrades + trade->getProfit();
      if (trade->getProfit() < test.getMaxLossTrade())
      {
	test.setMaxLossTrade(trade->getProfit());
	if (trade->getType() == 0)
	  test.setMaxLossLong(test.getMaxLossTrade());
	else
	  test.setMaxLossShort(test.getMaxLossTrade());
      }
    }

    trade->getEntryLogMessage(s);
    tradeLog->append(s);
    trade->getExitLogMessage(s);
    tradeLog->append(s);
  }

  test.setTotalComm((entryComm->value() + exitComm->value()) * trades.count());

  test.setNetProfit(test.getGrossProfit() - test.getTotalComm());

  if (trades.count())
    test.setAvgDrawDown(totalDraw / trades.count());

  test.setTrades(trades.count());

  if (test.getGrossProfit() && test.getWinTrades())
    test.setAvgWinTrade(test.getGrossProfit() / test.getWinTrades());

  if (totalLossTrades && test.getLossTrades())
    test.setAvgLossTrade(totalLossTrades / test.getLossTrades());

  if (test.getWinTrades() && test.getLossTrades())
    test.setWinLossRatio(test.getWinTrades() / test.getLossTrades());

  if (test.getWinTrades() && test.getTrades())
    test.setProfitable((test.getWinTrades() / test.getTrades()) * 100);

  grossProfit->setNum(test.getGrossProfit());
  netProfit->setNum(test.getNetProfit());
  maxDrawDown->setNum(test.getMaxDrawDown());
  avgDrawDown->setNum(test.getAvgDrawDown());
  commissions->setNum(test.getTotalComm());
  winLossRatio->setNum(test.getWinLossRatio());
  totalTrades->setNum(test.getTrades());
  percentProfitable->setNum(test.getProfitable());
  winningTrades->setNum(test.getWinTrades());
  losingTrades->setNum(test.getLossTrades());
  maxWinTrade->setNum(test.getMaxWinTrade());
  maxLossTrade->setNum(test.getMaxLossTrade());
  avgWinTrade->setNum(test.getAvgWinTrade());
  avgLossTrade->setNum(test.getAvgLossTrade());
  maxWinLong->setNum(test.getMaxWinLong());
  maxLossLong->setNum(test.getMaxLossLong());
  maxWinShort->setNum(test.getMaxWinShort());
  maxLossShort->setNum(test.getMaxLossShort());

  saveTest();
}

void QtStalkerTester::runTrades (BarData *data, QList<TestTrade *> &trades)
{
  PlotLine *line = data->getInput((BarData::InputType) exitField->currentIndex());
  if (! line)
    return;

  int loop;
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    trade->update(line);
  }

  delete line;
}

