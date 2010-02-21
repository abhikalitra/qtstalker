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
#include <QSqlQuery>

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
#include "IndicatorFactory.h"
#include "Indicator.h"
#include "Setting.h"

QtStalkerTester::QtStalkerTester ()
{
  enterLongIndicator = 0;
  exitLongIndicator = 0;
  enterShortIndicator = 0;
  exitShortIndicator = 0;

  priceList << "Open" << "High" << "Low" << "Close" << "AvgPrice" << "MedianPrice" << "TypicalPrice";

  IndicatorBase ib;
  ib.getIndicatorList(indicatorList, 1);

  BarData bd;
  bd.getBarLengthList(barLengthList);

//  setWindowIcon(QIcon());
  QApplication::setOrganizationName("QtStalkerTester");
  setWindowTitle("QtStalkerTester: " +  tr("Empty"));

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

  createRankTab();
  createRuleTab();
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
  if (enterLongIndicator)
    delete enterLongIndicator;

  if (exitLongIndicator)
    delete exitLongIndicator;

  if (enterShortIndicator)
    delete enterShortIndicator;

  if (exitShortIndicator)
    delete exitShortIndicator;
}

void QtStalkerTester::createRankTab()
{
  QWidget *w = new QWidget(this);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  w->setLayout(vbox);

  rankTree = new QTreeWidget;
  rankTree->setColumnCount(4);
  rankTree->setRootIsDecorated(FALSE);
  QStringList l;
  l << tr("Test Name") << tr("Win/Loss Ratio") << tr("Gross Profit") << tr("Net Profit");
  rankTree->setHeaderLabels(l);
  vbox->addWidget(rankTree);

  updateRankings();

  tabs->addTab(w, tr("Rank"));
}

void QtStalkerTester::createRuleTab()
{
  QWidget *w = new QWidget(this);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(10);
  w->setLayout(hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  hbox->addLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  vbox->addLayout(grid);
  int row = 0;
  int col = 0;

  // script box area
  scriptCheck = new QGroupBox;
  scriptCheck->setCheckable(TRUE);
  scriptCheck->setTitle(tr("Run Script"));
  scriptCheck->setChecked(FALSE);
  connect(scriptCheck, SIGNAL(toggled(bool)), this, SLOT(scriptCheckChanged(bool)));
  grid->addWidget(scriptCheck, row++, col);

    QGridLayout *tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    scriptCheck->setLayout(tgrid);

    QLabel *label = new QLabel(tr("Shell Command"));
    tgrid->addWidget(label, 0, 0);

    shellCommand = new QLineEdit;
    tgrid->addWidget(shellCommand, 0, 1);

    label = new QLabel(tr("Script File"));
    tgrid->addWidget(label, 1, 0);

    script = new QLineEdit;
    tgrid->addWidget(script, 1, 1);

    scriptButton = new QToolButton;
    scriptButton->setText(QString("..."));
    tgrid->addWidget(scriptButton, 1, 2);
    connect(scriptButton, SIGNAL(pressed()), this, SLOT(scriptButtonPressed()));


  // long box area
  longCheck = new QGroupBox;
  longCheck->setCheckable(TRUE);
  longCheck->setTitle(tr("Long"));
  longCheck->setChecked(FALSE);
  grid->addWidget(longCheck, row++, col);

    tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    longCheck->setLayout(tgrid);

    // enter long line
    label = new QLabel(tr("Enter Long"));
    tgrid->addWidget(label, 0, 0);

    enterLongCombo = new QComboBox;
    connect(enterLongCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(enterLongComboChanged()));
    enterLongCombo->addItems(indicatorList);
    tgrid->addWidget(enterLongCombo, 0, 1);

    enterLongButton = new QToolButton;
    tgrid->addWidget(enterLongButton, 0, 2);
    enterLongButton->setText(QString("..."));
    connect(enterLongButton, SIGNAL(pressed()), this, SLOT(enterLongButtonPressed()));

    // exit long line
    label = new QLabel(tr("Exit Long"));
    tgrid->addWidget(label, 1, 0);

    exitLongCombo = new QComboBox;
    connect(exitLongCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exitLongComboChanged()));
    exitLongCombo->addItems(indicatorList);
    tgrid->addWidget(exitLongCombo, 1, 1);

    exitLongButton = new QToolButton;
    tgrid->addWidget(exitLongButton, 1, 2);
    exitLongButton->setText(QString("..."));
    connect(exitLongButton, SIGNAL(pressed()), this, SLOT(exitLongButtonPressed()));

  // short box area
  shortCheck = new QGroupBox;
  shortCheck->setCheckable(TRUE);
  shortCheck->setTitle(tr("Short"));
  shortCheck->setChecked(FALSE);
  grid->addWidget(shortCheck, row++, col);

    tgrid = new QGridLayout;
    tgrid->setSpacing(5);
    tgrid->setColumnStretch(1, 1);
    shortCheck->setLayout(tgrid);

    // enter short line
    label = new QLabel(tr("Enter Short"));
    tgrid->addWidget(label, 0, 0);

    enterShortCombo = new QComboBox;
    connect(enterShortCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(enterShortComboChanged()));
    enterShortCombo->addItems(indicatorList);
    tgrid->addWidget(enterShortCombo, 0, 1);

    enterShortButton = new QToolButton;
    tgrid->addWidget(enterShortButton, 0, 2);
    enterShortButton->setText(QString("..."));
    connect(enterShortButton, SIGNAL(pressed()), this, SLOT(enterShortButtonPressed()));

    // exit short line
    label = new QLabel(tr("Exit Short"));
    tgrid->addWidget(label, 1, 0);

    exitShortCombo = new QComboBox;
    connect(exitShortCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(exitShortComboChanged()));
    exitShortCombo->addItems(indicatorList);
    tgrid->addWidget(exitShortCombo, 1, 1);

    exitShortButton = new QToolButton;
    tgrid->addWidget(exitShortButton, 1, 2);
    exitShortButton->setText(QString("..."));
    connect(exitShortButton, SIGNAL(pressed()), this, SLOT(exitShortButtonPressed()));

  // symbol parms
  grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);
  row = 0;
  col = 0;

  label = new QLabel(tr("Symbol"));
  grid->addWidget(label, row, col++);

//  QPushButton *pb = new QPushButton(QString::number(0) + tr(" Symbols"));
//  pb->setToolTip(tr("Select symbols for test"));
//  connect(pb, SIGNAL(pressed()), this, SLOT(symbolButtonPressed()));
  symbol = new QLineEdit;
  grid->addWidget(symbol, row++, col--);

  vbox->addStretch(1);

  // right side of page
  grid = new QGridLayout;
  grid->setSpacing(5);
  grid->setColumnStretch(1, 1);
  hbox->addLayout(grid);
  row = 0;
  col = 0;

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

  // trade delay
  label = new QLabel(tr("Trade Delay"));
  grid->addWidget(label, row, col++);

  delay = new QSpinBox;
  delay->setMinimum(0);
  delay->setMaximum(100);
  delay->setValue(1);
  grid->addWidget(delay, row++, col--);

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

  // trailing stop parms
  trailingCheck = new QCheckBox(tr("Trailing Stop"));
  grid->addWidget(trailingCheck, row, col++);

  trailing = new QDoubleSpinBox;
  trailing->setMinimum(0);
  trailing->setMaximum(100);
  trailing->setValue(7);
  grid->addWidget(trailing, row++, col--);

  grid->setRowStretch(row, 1);

  tabs->addTab(w, tr("Settings"));
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

  grossProfit = new QLabel("0");
  grid->addWidget(grossProfit, row++, col--);

  // net profit
  label = new QLabel(tr("Net Profit"));
  grid->addWidget(label, row, col++);

  netProfit = new QLabel("0");
  grid->addWidget(netProfit, row++, col--);

  // max drawdown
  label = new QLabel(tr("Maximum Drawdown"));
  grid->addWidget(label, row, col++);

  maxDrawDown = new QLabel("0");
  grid->addWidget(maxDrawDown, row++, col--);

  // avg drawdown
  label = new QLabel(tr("Average Drawdown"));
  grid->addWidget(label, row, col++);

  avgDrawDown = new QLabel("0");
  grid->addWidget(avgDrawDown, row++, col--);

  // commissions
  label = new QLabel(tr("Commissions"));
  grid->addWidget(label, row, col++);

  commissions = new QLabel("0");
  grid->addWidget(commissions, row++, col--);

  // win/loss ratio
  label = new QLabel(tr("Win/Loss Ratio"));
  grid->addWidget(label, row, col++);

  winLossRatio = new QLabel("0");
  grid->addWidget(winLossRatio, row++, col--);

  // total trades
  label = new QLabel(tr("Total Trades"));
  grid->addWidget(label, row, col++);

  totalTrades = new QLabel("0");
  grid->addWidget(totalTrades, row++, col--);

  // percent profitable
  label = new QLabel(tr("Percent Profitable"));
  grid->addWidget(label, row, col++);

  percentProfitable = new QLabel("0");
  grid->addWidget(percentProfitable, row++, col--);

  // winning trades
  label = new QLabel(tr("Winning Trades"));
  grid->addWidget(label, row, col++);

  winningTrades = new QLabel("0");
  grid->addWidget(winningTrades, row++, col--);

  // losing trades
  label = new QLabel(tr("Losing Trades"));
  grid->addWidget(label, row, col++);

  losingTrades = new QLabel("0");
  grid->addWidget(losingTrades, row++, col--);

  col = 2;
  row = 0;

  // maxWinTrade
  label = new QLabel(tr("Largest Winning Trade"));
  grid->addWidget(label, row, col++);

  maxWinTrade = new QLabel("0");
  grid->addWidget(maxWinTrade, row++, col--);

  // maxLosingTrade
  label = new QLabel(tr("Largest Losing Trade"));
  grid->addWidget(label, row, col++);

  maxLossTrade = new QLabel("0");
  grid->addWidget(maxLossTrade, row++, col--);

  // avgWinTrade
  label = new QLabel(tr("Average Winning Trade"));
  grid->addWidget(label, row, col++);

  avgWinTrade = new QLabel("0");
  grid->addWidget(avgWinTrade, row++, col--);

  // avgLosingTrade
  label = new QLabel(tr("Average Losing Trade"));
  grid->addWidget(label, row, col++);

  avgLossTrade = new QLabel("0");
  grid->addWidget(avgLossTrade, row++, col--);

  // maxWinLong
  label = new QLabel(tr("Largest Winning Long"));
  grid->addWidget(label, row, col++);

  maxWinLong = new QLabel("0");
  grid->addWidget(maxWinLong, row++, col--);

  // maxLosingLong
  label = new QLabel(tr("Largest Losing Long"));
  grid->addWidget(label, row, col++);

  maxLossLong = new QLabel("0");
  grid->addWidget(maxLossLong, row++, col--);

  // maxWinShort
  label = new QLabel(tr("Largest Winning Short"));
  grid->addWidget(label, row, col++);

  maxWinShort = new QLabel("0");
  grid->addWidget(maxWinShort, row++, col--);

  // maxLosingShort
  label = new QLabel(tr("Largest Losing Short"));
  grid->addWidget(label, row, col++);

  maxLossShort = new QLabel("0");
  grid->addWidget(maxLossShort, row++, col--);

  // balance
  label = new QLabel(tr("Account Balance"));
  grid->addWidget(label, row, col++);

  balance = new QLabel("0");
  grid->addWidget(balance, row++, col--);

  tradeLog = new QTreeWidget;
  tradeLog->setColumnCount(8);
  tradeLog->setRootIsDecorated(FALSE);
  QStringList l;
  l << tr("Enter Date") << tr("Enter Signal") << tr("Volume") << tr("Enter Price");
  l << tr("Exit Date") << tr("Exit Signal") << tr("Exit Price") << tr("Profit");
  tradeLog->setHeaderLabels(l);
  vbox->addWidget(tradeLog);

  tabs->addTab(w, tr("Results"));
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
  if (name.isEmpty())
    return;

  QString s = "INSERT OR REPLACE INTO results  VALUES (";
  s.append("'" + name + "'");

  if (script->text().isEmpty())
    s.append(",' '");
  else
    s.append(",'" + script->text() + "'");

  s.append("," + QString::number(scriptCheck->isChecked()));

  if (shellCommand->text().isEmpty())
    s.append(",' '");
  else
    s.append(",'" + shellCommand->text() + "'");

  s.append("," + QString::number(longCheck->isChecked()));
  s.append("," + QString::number(shortCheck->isChecked()));

  if (symbol->text().isEmpty())
    s.append(",' '");
  else
    s.append(",'" + symbol->text() + "'");

  s.append("," + QString::number(enterField->currentIndex()));
  s.append("," + QString::number(exitField->currentIndex()));
  s.append("," + QString::number(bars->value()));
  s.append("," + QString::number(barLength->currentIndex()));
  s.append("," + QString::number(entryComm->value()));
  s.append("," + QString::number(exitComm->value()));
  s.append("," + QString::number(delay->value()));
  s.append("," + QString::number(account->value()));
  s.append("," + QString::number(volumePercentage->value()));
  s.append("," + QString::number(trailing->value()));
  s.append("," + QString::number(trailingCheck->isChecked()));
  s.append(",'" + enterLongCombo->currentText() + "'");
  s.append(",'" + exitLongCombo->currentText() + "'");
  s.append(",'" + enterShortCombo->currentText() + "'");
  s.append(",'" + exitShortCombo->currentText() + "'");

  if (enterLongIndicator)
  {
    Indicator ind;
    enterLongIndicator->getSettings(ind);
    Setting set;
    ind.getSettings(set);
    QString ts;
    set.getString(ts);
    s.append(",'" + ts + "'");
  }
  else
    s.append(",' '");

  if (exitLongIndicator)
  {
    Indicator ind;
    exitLongIndicator->getSettings(ind);
    Setting set;
    ind.getSettings(set);
    QString ts;
    set.getString(ts);
    s.append(",'" + ts + "'");
  }
  else
    s.append(",' '");

  if (enterShortIndicator)
  {
    Indicator ind;
    enterShortIndicator->getSettings(ind);
    Setting set;
    ind.getSettings(set);
    QString ts;
    set.getString(ts);
    s.append(",'" + ts + "'");
  }
  else
    s.append(",' '");

  if (exitShortIndicator)
  {
    Indicator ind;
    exitShortIndicator->getSettings(ind);
    Setting set;
    ind.getSettings(set);
    QString ts;
    set.getString(ts);
    s.append(",'" + ts + "'");
  }
  else
    s.append(",' '");

  // summary stuff
  s.append("," + grossProfit->text());
  s.append("," + netProfit->text());
  s.append("," + maxDrawDown->text());
  s.append("," + avgDrawDown->text());
  s.append("," + commissions->text());
  s.append("," + winLossRatio->text());
  s.append("," + totalTrades->text());
  s.append("," + percentProfitable->text());
  s.append("," + winningTrades->text());
  s.append("," + losingTrades->text());
  s.append("," + maxWinTrade->text());
  s.append("," + maxLossTrade->text());
  s.append("," + avgWinTrade->text());
  s.append("," + avgLossTrade->text());
  s.append("," + maxWinLong->text());
  s.append("," + maxLossLong->text());
  s.append("," + maxWinShort->text());
  s.append("," + maxLossShort->text());

  // get tradelog data
  QStringList l;
  int loop;
  for (loop = 0; loop < tradeLog->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = tradeLog->topLevelItem(loop);
    if (! item)
      continue;

    int loop2;
    QStringList l2;
    for (loop2 = 0; loop2 < item->columnCount(); loop2++)
      l2.append(item->text(loop2));

    l.append(l2.join(","));
  }
  if (l.count())
    s.append(",'" + l.join("\n") + "'");
  else
    s.append(",' '");

  s.append("," + balance->text());
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

void QtStalkerTester::scriptButtonPressed ()
{
  QString dir;
  TestConfig config;
  config.getData(TestConfig::ScriptDirectory, dir);
  if (dir.isEmpty())
    dir = QDir::homePath();

  QString s = QFileDialog::getOpenFileName(this,
					   QString(tr("Select Test Script")),
					   dir,
					   QString(),
					   0,
					   0);
  if (s.isEmpty())
    return;

  config.setData(TestConfig::ScriptDirectory, s);

  script->setText(s);
}

void QtStalkerTester::scriptCheckChanged (bool d)
{
  if (! d)
  {
    longCheck->setEnabled(TRUE);
    shortCheck->setEnabled(TRUE);
  }
  else
  {
    longCheck->setEnabled(FALSE);
    shortCheck->setEnabled(FALSE);
  }
}

void QtStalkerTester::symbolButtonPressed ()
{
  // custom symbol selection dialog here
}

void QtStalkerTester::enterLongComboChanged ()
{
  if (enterLongIndicator)
    delete enterLongIndicator;

  IndicatorFactory fac;
  QString s = enterLongCombo->currentText();
  enterLongIndicator = fac.getFunction(s);
}

void QtStalkerTester::exitLongComboChanged ()
{
  if (exitLongIndicator)
    delete exitLongIndicator;

  IndicatorFactory fac;
  QString s = exitLongCombo->currentText();
  exitLongIndicator = fac.getFunction(s);
}

void QtStalkerTester::enterShortComboChanged ()
{
  if (enterShortIndicator)
    delete enterShortIndicator;

  IndicatorFactory fac;
  QString s = enterShortCombo->currentText();
  enterShortIndicator = fac.getFunction(s);
}

void QtStalkerTester::exitShortComboChanged ()
{
  if (exitShortIndicator)
    delete exitShortIndicator;

  IndicatorFactory fac;
  QString s = exitShortCombo->currentText();
  exitShortIndicator = fac.getFunction(s);
}

void QtStalkerTester::enterLongButtonPressed ()
{
  enterLongIndicator->dialog(1);
}

void QtStalkerTester::exitLongButtonPressed ()
{
  exitLongIndicator->dialog(1);
}

void QtStalkerTester::enterShortButtonPressed ()
{
  enterShortIndicator->dialog(1);
}

void QtStalkerTester::exitShortButtonPressed ()
{
  exitShortIndicator->dialog(1);
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
    script->setText(q.value(col++).toString());
    scriptCheck->setChecked(q.value(col++).toInt());
    shellCommand->setText(q.value(col++).toString());
    longCheck->setChecked(q.value(col++).toInt());
    shortCheck->setChecked(q.value(col++).toInt());
    symbol->setText(q.value(col++).toString());
    enterField->setCurrentIndex(q.value(col++).toInt());
    exitField->setCurrentIndex(q.value(col++).toInt());
    bars->setValue(q.value(col++).toInt());
    barLength->setCurrentIndex(q.value(col++).toInt());
    entryComm->setValue(q.value(col++).toDouble());
    exitComm->setValue(q.value(col++).toDouble());
    delay->setValue(q.value(col++).toInt());
    account->setValue(q.value(col++).toDouble());
    volumePercentage->setValue(q.value(col++).toDouble());
    trailing->setValue(q.value(col++).toDouble());
    trailingCheck->setChecked(q.value(col++).toInt());
    enterLongCombo->setCurrentIndex(indicatorList.indexOf(q.value(col++).toString()));
    exitLongCombo->setCurrentIndex(indicatorList.indexOf(q.value(col++).toString()));
    enterShortCombo->setCurrentIndex(indicatorList.indexOf(q.value(col++).toString()));
    exitShortCombo->setCurrentIndex(indicatorList.indexOf(q.value(col++).toString()));

    Indicator ind;
    Setting set;
    QString s = q.value(col++).toString();
    set.parse(s);
    ind.setSettings(set);
    enterLongIndicator->setSettings(ind);

    set.clear();
    s = q.value(col++).toString();
    set.parse(s);
    ind.setSettings(set);
    exitLongIndicator->setSettings(ind);

    set.clear();
    s = q.value(col++).toString();
    set.parse(s);
    ind.setSettings(set);
    enterShortIndicator->setSettings(ind);

    set.clear();
    s = q.value(col++).toString();
    set.parse(s);
    ind.setSettings(set);
    exitShortIndicator->setSettings(ind);

    // summary stuff
    grossProfit->setNum(q.value(col++).toDouble());
    netProfit->setNum(q.value(col++).toDouble());
    maxDrawDown->setNum(q.value(col++).toDouble());
    avgDrawDown->setNum(q.value(col++).toDouble());
    commissions->setNum(q.value(col++).toDouble());
    winLossRatio->setNum(q.value(col++).toDouble());
    totalTrades->setNum(q.value(col++).toInt());
    percentProfitable->setNum(q.value(col++).toDouble());
    winningTrades->setNum(q.value(col++).toInt());
    losingTrades->setNum(q.value(col++).toInt());
    maxWinTrade->setNum(q.value(col++).toDouble());
    maxLossTrade->setNum(q.value(col++).toDouble());
    avgWinTrade->setNum(q.value(col++).toDouble());
    avgLossTrade->setNum(q.value(col++).toDouble());
    maxWinLong->setNum(q.value(col++).toDouble());
    maxLossLong->setNum(q.value(col++).toDouble());
    maxWinShort->setNum(q.value(col++).toDouble());
    maxLossShort->setNum(q.value(col++).toDouble());

    // summary items
    tradeLog->clear();
    tradeLog->setSortingEnabled(FALSE);

    QStringList l = q.value(col++).toString().split("\n");
    int loop;
    for (loop = 0; loop < l.count(); loop++)
    {
      QStringList l2 = l[loop].split(",");
      int loop2;
      QTreeWidgetItem *item = new QTreeWidgetItem(tradeLog);
      for (loop2 = 0; loop2 < l2.count(); loop2++)
        item->setText(loop2, l2[loop2]);
    }

    for (loop = 0; loop < tradeLog->columnCount(); loop++)
      tradeLog->resizeColumnToContents(loop);
    tradeLog->setSortingEnabled(TRUE);

    balance->setNum(q.value(col++).toDouble());
  }
}

void QtStalkerTester::cancelTest ()
{
}

void QtStalkerTester::run ()
{
  if (scriptCheck->isChecked())
  {
    QString s = script->text();
    if (s.isEmpty())
    {
      QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing script file."));
      return;
    }

    s = shellCommand->text();
    if (s.isEmpty())
    {
      QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing shell command."));
      return;
    }
  }

  QString s = symbol->text();
  if (s.isEmpty())
  {
    QMessageBox::information(this, tr("QtStalkerTest: Error"), tr("Missing symbol."));
    return;
  }

  BarData *data = new BarData;
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

  TestSignal enterLongSigs;
  TestSignal exitLongSigs;
  TestSignal enterShortSigs;
  TestSignal exitShortSigs;

  if (scriptCheck->isChecked())
  {
    ExScript server;
    server.setBarData(data);
    server.setDeleteFlag(TRUE);
    s = shellCommand->text() + " " + script->text();
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
    if (longCheck->isChecked())
    {
      if (enterLongIndicator->test(data, enterLongSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: enterLong error";
        return;
      }

      if (exitLongIndicator->test(data, exitLongSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: exitLong error";
        return;
      }
    }

    if (shortCheck->isChecked())
    {
      if (enterShortIndicator->test(data, enterShortSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: enterShort error";
        return;
      }

      if (exitShortIndicator->test(data, exitShortSigs))
      {
        qDebug() << "QtStalkerTester::runDialog: exitShort error";
        return;
      }
    }
  }

  QList<TestTrade *> trades;
  TestTrade ttrade;
  if (scriptCheck->isChecked())
  {
    ttrade.createTrades(data, trades, 0, volumePercentage->value(), delay->value(),
		        enterField->currentIndex(), enterLongSigs, exitLongSigs);
    ttrade.createTrades(data, trades, 1, volumePercentage->value(), delay->value(),
	                exitField->currentIndex(), enterShortSigs, exitShortSigs);
  }
  else
  {
    if (longCheck->isChecked())
    {
      ttrade.createTrades(data, trades, 0, volumePercentage->value(), delay->value(),
		          enterField->currentIndex(), enterLongSigs, exitLongSigs);
    }

    if (shortCheck->isChecked())
    {
      ttrade.createTrades(data, trades, 1, volumePercentage->value(), delay->value(),
	                  exitField->currentIndex(), enterShortSigs, exitShortSigs);
    }
  }

  runTrades(data, trades);

  createSummary(trades);

  updateRankings();

  delete data;
  qDeleteAll(trades);
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
  double tprofit = 0;
  int loop;
  QString s;

  tradeLog->clear();
  tradeLog->setSortingEnabled(FALSE);

  for (loop = 0; loop < trades.count(); loop++)
  {
    TestTrade *trade = trades.at(loop);

    tprofit = tprofit + trade->getProfit();

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
	tmaxWinTrade = trade->getProfit();

      if (trade->getType() == 0)
      {
	if (trade->getProfit() > tmaxWinLong)
          tmaxWinLong = trade->getProfit();
      }
      else
      {
	if (trade->getProfit() > tmaxWinShort)
	  tmaxWinShort = trade->getProfit();
      }
    }
    else
    {
      lossTrades++;

      totalLossTrades = totalLossTrades + trade->getProfit();

      if (trade->getProfit() < tmaxLossTrade)
	tmaxLossTrade = trade->getProfit();

      if (trade->getType() == 0)
      {
	if (trade->getProfit() < tmaxLossLong)
	  tmaxLossLong = trade->getProfit();
      }
      else
      {
	if (trade->getProfit() < tmaxLossShort)
	  tmaxLossShort = trade->getProfit();
      }
    }

    QStringList l;
    trade->getLogMessage(l);

    int loop2;
    QTreeWidgetItem *item = new QTreeWidgetItem(tradeLog);
    for (loop2 = 0; loop2 < l.count(); loop2++)
      item->setText(loop2, l[loop2]);
  }

  for (loop = 0; loop < tradeLog->columnCount(); loop++)
    tradeLog->resizeColumnToContents(loop);
  tradeLog->setSortingEnabled(TRUE);

  // commissions
  commission = (entryComm->value() + exitComm->value()) * trades.count();

  // net
  net = tprofit - commission;

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

  grossProfit->setNum(gross);
  netProfit->setNum(net);
  maxDrawDown->setNum(tmaxDrawDown);
  avgDrawDown->setNum(tavgDrawDown);
  commissions->setNum(commission);
  winLossRatio->setNum(twinLossRatio);
  totalTrades->setNum(trades.count());
  percentProfitable->setNum(profitable);
  winningTrades->setNum(winTrades);
  losingTrades->setNum(lossTrades);
  maxWinTrade->setNum(tmaxWinTrade);
  maxLossTrade->setNum(tmaxLossTrade);
  avgWinTrade->setNum(tavgWinTrade);
  avgLossTrade->setNum(tavgLossTrade);
  maxWinLong->setNum(tmaxWinLong);
  maxLossLong->setNum(tmaxLossLong);
  maxWinShort->setNum(tmaxWinShort);
  maxLossShort->setNum(tmaxLossShort);
  balance->setNum(tbalance);

  saveTest();
}

void QtStalkerTester::updateRankings ()
{
  TestDataBase db;
  QSqlQuery q;
  db.getRankings(q);

  rankTree->clear();

  rankTree->setSortingEnabled(FALSE);

  while (q.next())
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(rankTree);
    item->setText(0, q.value(0).toString());
    item->setText(1, q.value(1).toString());
    item->setText(2, q.value(2).toString());
    item->setText(3, q.value(3).toString());
  }

  int loop;
  for (loop = 0; loop < rankTree->columnCount(); loop++)
    rankTree->resizeColumnToContents(loop);

  rankTree->setSortingEnabled(TRUE);
}

