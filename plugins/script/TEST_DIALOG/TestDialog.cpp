/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#include "TestDialog.h"
#include "Globals.h"
#include "TestDataBase.h"

#include <QtDebug>
#include <QSettings>
#include <QGroupBox>
#include <QToolBar>

TestDialog::TestDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "test_dialog_window_size";
  _keyPos = "test_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Test Summary");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void TestDialog::createGUI ()
{
  int pos = 0;
  
  QToolBar *toolBar = new QToolBar;
  _vbox->insertWidget(pos++, toolBar);

  _tests = new QComboBox;
  _tests->setToolTip(tr("Test Name"));
  connect(_tests, SIGNAL(activated(int)), this, SLOT(update()));
  toolBar->addWidget(_tests);

  QStringList l;
  l << tr("Symbol") << tr("Type") << tr("Volume") << tr("Enter Date") << tr("Enter Price") << tr("Exit Date");
  l << tr("Exit Price") << tr("Profit") << tr("Signal") << tr("Equity");

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Trades"));
  _vbox->insertWidget(pos++, gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _tradeList = new QTreeWidget;
  _tradeList->setSortingEnabled(TRUE);
  _tradeList->setRootIsDecorated(FALSE);
  _tradeList->setHeaderLabels(l);
  _tradeList->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_tradeList);

  gbox = new QGroupBox;
  gbox->setTitle(tr("Report"));
  _vbox->insertWidget(pos++, gbox);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _report = new QTextEdit;
  _report->setReadOnly(TRUE);
  tvbox->addWidget(_report);

  _message->hide();
}

void TestDialog::loadSettings ()
{
  Dialog::loadSettings();

  TestDataBase db;
  QStringList l;
  db.names(l);
  _tests->addItems(l);

  QSettings settings(g_localSettings);
  _tests->setCurrentIndex(settings.value("test_dialog_last_test").toInt());
}

void TestDialog::saveSettings ()
{
  Dialog::saveSettings();

  QSettings settings(g_localSettings);
  settings.setValue("test_dialog_last_test", _tests->currentIndex());
  settings.sync();
}

void TestDialog::done ()
{
  _command->setReturnCode("0");
  saveSettings();
  accept();
}

void TestDialog::update ()
{
  TestDataBase db;
  Setting set;
  set.setData("NAME", _tests->currentText());

  QList<Setting> trades;
  if (db.trades(set, trades))
  {
    qDebug() << "TestDialog::updateTrades: db error loading trades";
    return;
  }

  updateTrades(trades);

  updateReport(trades);
}

void TestDialog::updateTrades (QList<Setting> &trades)
{
  _tradeList->clear();
  
  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    Setting trade = trades.at(loop);
    
    QStringList l;
    l << trade.data("SYMBOL") << trade.data("TYPE") << trade.data("VOLUME") << trade.data("ENTER_DATE");
    l << trade.data("ENTER_PRICE") << trade.data("EXIT_DATE") << trade.data("EXIT_PRICE");
    l << trade.data("PROFIT") << trade.data("SIGNAL") << trade.data("EQUITY");
    new QTreeWidgetItem(_tradeList, l);
  }

  for (loop = 0; loop < _tradeList->columnCount(); loop++)
    _tradeList->resizeColumnToContents(loop);
}

void TestDialog::updateReport (QList<Setting> &trades)
{
  _report->clear();

  int win = 0;
  int loss = 0;
  double winTotal = 0;
  double lossTotal = 0;
  double profit = 0;
  double drawDown = 0;
  int maxBars = 0;
  int minBars = 99999999;
  int totalBars = 0;
  double commissions = 0;
  double sequity = 0;
  double equity = 0;
  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    Setting t = trades.at(loop);

    if (t.getDouble("PROFIT") < 0)
    {
      loss++;
      lossTotal += t.getDouble("PROFIT");
    }
    else
    {
      win++;
      winTotal += t.getDouble("PROFIT");
    }

    profit += t.getDouble("PROFIT");

    int bars = t.getInt("BARS_HELD");
    totalBars += bars;
    if (bars < minBars)
      minBars = bars;
    if (bars > maxBars)
      maxBars = bars;

    if (t.getDouble("DRAWDOWN") < drawDown)
      drawDown = t.getDouble("DRAWDOWN");

    commissions += t.getDouble("ENTER_COMM");
    commissions += t.getDouble("EXIT_COMM");

    if (sequity == 0)
      sequity = t.getDouble("EQUITY") + profit + commissions;

    equity = t.getDouble("EQUITY");
  }

  double t = 0;
  if (winTotal == 0 || lossTotal == 0)
    t = 0;
  else
    t = (double) (winTotal / lossTotal);
  _report->append(tr("Profit Factor") + ": \t" + QString::number(t, 'f', 2));

  if (winTotal == 0 || lossTotal == 0)
    t = 0;
  else
    t = (double) ((winTotal / (double) win) / (lossTotal / (double) loss));
  _report->append(tr("Payoff Ratio") + ": \t" + QString::number(t, 'f', 2));

  t = (double) ((equity - sequity) / sequity) * 100;
  _report->append(tr("% Equity Gain") + ": \t" + QString::number(t, 'f', 2));

  _report->append(tr("Equity") + ": \t\t" + QString::number(equity, 'f', 2));

  _report->append(tr("Total Profit") + ": \t" + QString::number(profit, 'f', 2));

  t = (double) (((double) win / (double) trades.count()) * 100);
  _report->append(tr("% Profitable Trades") + ": \t" + QString::number(t, 'f', 2));

  _report->append(tr("Total Trades") + ": \t" + QString::number(trades.count()));

  _report->append(tr("Winning Trades") + ": \t" + QString::number(win));

  _report->append(tr("Losing Trades") + ": \t" + QString::number(loss));

  _report->append(tr("Maximum Drawdown") + ": \t" + QString::number(drawDown, 'f', 2));

  t = (double) (profit / (double) trades.count());
  _report->append(tr("Avg Profit/Loss") + ": \t" + QString::number(t, 'f', 2));

  _report->append(tr("Total Winning Trades") + ": \t" + QString::number(winTotal, 'f', 2));

  _report->append(tr("Total Losing Trades") + ": \t" + QString::number(lossTotal, 'f', 2));

  t = (int) totalBars / trades.count();
  _report->append(tr("Avg Bars Held") + ": \t" + QString::number(t, 'f', 2));

  _report->append(tr("Min Bars Held") + ": \t" + QString::number(minBars));

  _report->append(tr("Max Bars Held") + ": \t" + QString::number(maxBars));

  _report->append(tr("Total Commissions") + ": \t" + QString::number(commissions, 'f', 2));
}
