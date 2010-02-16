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
  QString dbFile;
  config.getData(TestConfig::DbName, dbFile);
  qdb.init(dbFile);

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
  QLabel *label = new QLabel(tr("Script Command"));
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
  barLength->setCurrentIndex(6);
  grid->addWidget(barLength, row++, col--);

  // bars
  label = new QLabel(tr("Bars"));
  grid->addWidget(label, row, col++);

  bars = new QSpinBox;
  bars->setMinimum(1);
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

  // trade delay
  label = new QLabel(tr("Trade Delay"));
  grid->addWidget(label, row, col++);

  delay = new QSpinBox;
  delay->setMinimum(0);
  delay->setMaximum(100);
  delay->setValue(1);
  grid->addWidget(delay, row++, col--);

  // account
  label = new QLabel(tr("Account"));
  grid->addWidget(label, row, col++);

  account = new QDoubleSpinBox;
  account->setMinimum(0);
  account->setMaximum(99999999);
  account->setValue(10000);
  grid->addWidget(account, row++, col--);

  // volume percentage
  label = new QLabel(tr("Volume Percentage"));
  grid->addWidget(label, row, col++);

  volumePercentage = new QDoubleSpinBox;
  volumePercentage->setMinimum(0);
  volumePercentage->setMaximum(100);
  volumePercentage->setValue(20);
  grid->addWidget(volumePercentage, row++, col--);

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

  // balance
  label = new QLabel(tr("Account Balance"));
  grid->addWidget(label, row, col++);

  balance = new QLabel;
  grid->addWidget(balance, row++, col--);

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

  enterField->setCurrentIndex(test.getEnterField());

  exitField->setCurrentIndex(test.getExitField());

  barLength->setCurrentIndex(test.getBarLength());

  bars->setValue(test.getBars());

  entryComm->setValue(test.getEntryComm());

  exitComm->setValue(test.getExitComm());

  delay->setValue(test.getDelay());

  account->setValue(test.getAccount());

  volumePercentage->setValue(test.getVolumePercentage());

  trailingCheck->setChecked(test.getTrailingCheck());

  trailing->setValue(test.getTrailingStop());

  // summary items
  test.getTradeLog(ts);
  tradeLog->append(ts);
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
  balance->setNum(test.getBalance());
}

void QtStalkerTester::getSettings ()
{
  QString ts;
//  test.setScript(scriptFile);
  ts = script->text();
  test.setScript(ts);

  ts = symbol->text();
  test.setSymbol(ts);

  test.setEnterField(enterField->currentIndex());

  test.setExitField(exitField->currentIndex());

  test.setBarLength(barLength->currentIndex());

  test.setBars(bars->value());

  test.setEntryComm(entryComm->value());

  test.setExitComm(exitComm->value());

  test.setDelay(delay->value());

  test.setAccount(account->value());

  test.setVolumePercentage(volumePercentage->value());

  test.setTrailingCheck(trailingCheck->isChecked());

  test.setTrailingStop(trailing->value());
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
  qdb.getChart(data);
  if (data->count() == 0)
  {
    qDebug() << "QtStalkerTester::run: 0 bars loaded";
    return;
  }

  ExScript server;
  server.setBarData(data);
  server.setDeleteFlag(TRUE);
  test.getScript(s);
  server.calculate(s); // we are calling the blocking version of calculate() here

  QHash<int, TestSignal *> enterLongSigs;
  PlotLine *l = server.getEnterLong();
  if (l)
    createSignals(data, l, enterLongSigs);

  QHash<int, TestSignal *> exitLongSigs;
  l = server.getExitLong();
  if (l)
    createSignals(data, l, exitLongSigs);

  QHash<int, TestSignal *> enterShortSigs;
  l = server.getEnterShort();
  if (l)
    createSignals(data, l, enterShortSigs);

  QHash<int, TestSignal *> exitShortSigs;
  l = server.getExitShort();
  if (l)
    createSignals(data,l, exitShortSigs);

  QList<TestTrade *> trades;
  createTrades(data, trades, 0, enterLongSigs, exitLongSigs);
  createTrades(data, trades, 1, enterShortSigs, exitShortSigs);

  runTrades(data, trades);

  createSummary(trades);

  delete data;
  qDeleteAll(trades);
  qDeleteAll(enterLongSigs);
  qDeleteAll(exitLongSigs);
  qDeleteAll(enterShortSigs);
  qDeleteAll(exitShortSigs);
}

void QtStalkerTester::createSignals (BarData *data, PlotLine *line, QHash<int, TestSignal*> &sigs)
{
  int barLoop = data->count() - line->getSize();
  if (barLoop < 0)
  {
    qDebug() << "QtStalkerTester::createSignals" << line->getSize() << "size > bars size";
    return;
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
          TestSignal *sig = new TestSignal;
	  sig->setBar(barLoop);
	  sigs.insert(barLoop, sig);
	  QDateTime dt;
	  data->getDate(barLoop, dt);
	}
	break;
    }
  }
}

void QtStalkerTester::createTrades (BarData *data, QList<TestTrade *> &trades, int type,
				    QHash<int, TestSignal *> &enterSigs, QHash<int, TestSignal *> &exitSigs)
{
  int loop;
  int status = 0;
  for (loop = 0; loop < data->count(); loop++)
  {
    switch (status)
    {
      case 1: // currently in trade, check if we exit trade
      {
	TestSignal *sig = exitSigs.value(loop);
	if (sig)
	{
	  status = 0; // exit
	  TestTrade *trade = trades.at(trades.count() - 1);
	  endTrade(loop, data, trade);
	}
	break;
      }
      default:
      {
	TestSignal *sig = enterSigs.value(loop);
	if (sig)
	{
	  status = 1; // we are in a trade
	  TestTrade *trade = new TestTrade;
	  trade->setType(type);
          trade->setVolumePercentage(volumePercentage->value());
	  startTrade(loop, data, trade);
	  trades.append(trade);
	}
	break;
      }
    }
  }
}

void QtStalkerTester::startTrade (int pos, BarData *data, TestTrade *trade)
{
  int offset = pos + delay->value();
  if (offset >= data->count())
    offset = pos;

  QDateTime dt;
  data->getDate(offset, dt);
  trade->setEnterDate(dt, offset);

  int i = enterField->currentIndex();

  switch (i)
  {
    case 0:
      trade->setEnterPrice(data->getOpen(offset));
      break;
    case 1:
      trade->setEnterPrice(data->getHigh(offset));
      break;
    case 2:
      trade->setEnterPrice(data->getLow(offset));
      break;
    case 4:
      trade->setEnterPrice(data->getAvgPrice(offset));
      break;
    case 5:
      trade->setEnterPrice(data->getMedianPrice(offset));
      break;
    case 6:
      trade->setEnterPrice(data->getTypicalPrice(offset));
      break;
    default:
      trade->setEnterPrice(data->getClose(offset));
      break;
  }
}

void QtStalkerTester::endTrade (int pos, BarData *data, TestTrade *trade)
{
  int offset = pos + delay->value();
  if (offset >= data->count())
    offset = pos;

  QDateTime dt;
  data->getDate(offset, dt);
  trade->setExitDate(dt, offset);

  int i = exitField->currentIndex();

  switch (i)
  {
    case 0:
      trade->setExitPrice(data->getOpen(offset));
      break;
    case 1:
      trade->setExitPrice(data->getHigh(offset));
      break;
    case 2:
      trade->setExitPrice(data->getLow(offset));
      break;
    case 4:
      trade->setExitPrice(data->getAvgPrice(offset));
      break;
    case 5:
      trade->setExitPrice(data->getMedianPrice(offset));
      break;
    case 6:
      trade->setExitPrice(data->getTypicalPrice(offset));
      break;
    default:
      trade->setExitPrice(data->getClose(offset));
      break;
  }
}

void QtStalkerTester::runTrades (BarData *data, QList<TestTrade *> &trades)
{

  PlotLine *line = 0;
  switch (exitField->currentIndex())
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
  double money = account->value();
  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);
    if (trailingCheck->isChecked())
      trade->setTrailing(trailing->value());
    trade->update(line, data, money);
    money = money + trade->getProfit();
  }

  delete line;
}

void QtStalkerTester::createSummary (QList<TestTrade *> &trades)
{
  double totalDraw = 0;
  double tmaxDrawDown = 0;
  int longTrades = 0;
  int winTrades = 0;
  int lossTrades = 0;
  int shortTrades = 0;
  double totalLossTrades = 0;
  int numDrawDown = 0;
  double gross = 0;
  double tmaxWinTrade = 0;
  double tmaxLossTrade = 0;
  double tmaxWinLong = 0;
  double tmaxLossLong = 0;
  double tmaxWinShort = 0;
  double tmaxLossShort = 0;
  double commission = 0;
  double net = 0;
  double tavgDrawDown = 0;
  double tavgWinTrade = 0;
  double tavgLossTrade = 0;
  double twinLossRatio = 0;
  double profitable = 0;
  double tbalance = account->value();
  int loop;
  QString s;

  tradeLog->clear();
  QStringList tradeLogList;

  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);

    if (trade->getDrawDown() < 0)
    {
      totalDraw = totalDraw + trade->getDrawDown();
      numDrawDown++;
    }

    if (trade->getDrawDown() < tmaxDrawDown)
      tmaxDrawDown = trade->getDrawDown();

    if (trade->getType() == 0)
      longTrades++;
    else
      shortTrades++;

    if (trade->getProfit() >= 0)
    {
      winTrades++;
      gross = gross + trade->getProfit();
      if (trade->getProfit() > tmaxWinTrade)
      {
	tmaxWinTrade = trade->getProfit();
	if (trade->getType() == 0)
	  tmaxWinLong = tmaxWinTrade;
	else
	  tmaxWinShort = tmaxWinTrade;
      }
    }
    else
    {
      lossTrades++;
      totalLossTrades = totalLossTrades + trade->getProfit();
      if (trade->getProfit() < tmaxLossTrade)
      {
	tmaxLossTrade = trade->getProfit();
	if (trade->getType() == 0)
	  tmaxLossLong = tmaxLossTrade;
	else
	  tmaxLossShort = tmaxLossTrade;
      }
    }

    trade->getLogMessage(s);
    tradeLogList.append(s);
  }

  tradeLogList.sort();
  for (loop = 0; loop < tradeLogList.count(); loop++)
    tradeLog->append(tradeLogList[loop]);

  // commissions
  commission = (entryComm->value() + exitComm->value()) * trades.count();

  // net
  net = gross - commission;

  tbalance = tbalance + net;

  // avg draw down
  if (numDrawDown)
    tavgDrawDown = totalDraw / (double) numDrawDown;

  // avg win trade
  if (gross && winTrades)
    tavgWinTrade = gross / (double) winTrades;

  // avg loss trade
  if (totalLossTrades && lossTrades)
    tavgLossTrade = totalLossTrades / (double) lossTrades;

  // avg win / avg loss ratio
  if (tavgWinTrade && tavgLossTrade)
  {
    twinLossRatio = tavgWinTrade / tavgLossTrade;
    if (twinLossRatio < 0)
      twinLossRatio = twinLossRatio * -1;
  }

  // % profitable
  if (winTrades && trades.count())
    profitable = ((double) winTrades / (double) trades.count()) * 100;

  // update test and report fields
  s = tradeLog->toPlainText();
  test.setTradeLog(s);

  grossProfit->setNum(gross);
  test.setGrossProfit(gross);

  netProfit->setNum(net);
  test.setNetProfit(net);

  maxDrawDown->setNum(tmaxDrawDown);
  test.setMaxDrawDown(tmaxDrawDown);

  avgDrawDown->setNum(tavgDrawDown);
  test.setAvgDrawDown(tavgDrawDown);

  commissions->setNum(commission);
  test.setCommissions(commission);

  winLossRatio->setNum(twinLossRatio);
  test.setWinLossRatio(twinLossRatio);

  totalTrades->setNum(trades.count());
  test.setTrades(trades.count());

  percentProfitable->setNum(profitable);
  test.setProfitable(profitable);

  winningTrades->setNum(winTrades);
  test.setWinTrades(winTrades);

  losingTrades->setNum(lossTrades);
  test.setLossTrades(lossTrades);

  maxWinTrade->setNum(tmaxWinTrade);
  test.setMaxWinTrade(tmaxWinTrade);

  maxLossTrade->setNum(tmaxLossTrade);
  test.setMaxLossTrade(tmaxLossTrade);

  avgWinTrade->setNum(tavgWinTrade);
  test.setAvgWinTrade(tavgWinTrade);

  avgLossTrade->setNum(tavgLossTrade);
  test.setAvgLossTrade(tavgLossTrade);

  maxWinLong->setNum(tmaxWinLong);
  test.setMaxWinLong(tmaxWinLong);

  maxLossLong->setNum(tmaxLossLong);
  test.setMaxLossLong(tmaxLossLong);

  maxWinShort->setNum(tmaxWinShort);
  test.setMaxWinShort(tmaxWinShort);

  maxLossShort->setNum(tmaxLossShort);
  test.setMaxLossShort(tmaxLossShort);

  balance->setNum(tbalance);
  test.setBalance(tbalance);

  saveTest();
}



