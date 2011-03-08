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
#include "SummaryThread.h"
#include "../../../pics/refresh.xpm"

#include <QtDebug>
#include <QSettings>
#include <QGroupBox>
#include <QToolBar>
#include <QToolButton>
#include <QApplication>

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

  updateSummary();
}

void TestDialog::createGUI ()
{
  int pos = 0;

  QToolBar *tb = new QToolBar(this);
  _vbox->insertWidget(pos++, tb);

  QToolButton *b = new QToolButton(this);
  b->setIcon(QIcon(refresh_xpm));
  b->setToolTip(tr("Refresh Summary"));
  b->setStatusTip(tr("Refresh Summary"));
  connect(b, SIGNAL(clicked()), this, SLOT(updateSummary()));
  tb->addWidget(b);
  
  // summary
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Summary"));
  _vbox->insertWidget(pos++, gbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  gbox->setLayout(vbox);

  QStringList l;
  l << tr("Name");
  l << tr("Tag");
  l << tr("Symbol");
  l << tr("% Gain");
  l << tr("Equity");
  l << tr("T Profit");
  l << tr("% P Trades");
  l << tr("T Trades");
  l << tr("Win Trades");
  l << tr("Loss Trades");
  l << tr("Max Draw");
  l << tr("Avg P/L");
  l << tr("T Win Trades");
  l << tr("T Loss Trades");
  l << tr("Avg Bars");
  l << tr("Min Bars");
  l << tr("Max Bars");
  l << tr("T Comm.");

  _summary = new QTreeWidget;
  _summary->setSortingEnabled(TRUE);
  _summary->setRootIsDecorated(FALSE);
  _summary->setHeaderLabels(l);
  _summary->setSelectionMode(QAbstractItemView::SingleSelection);
  connect(_summary, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(summarySelected(QTreeWidgetItem *)));
  vbox->addWidget(_summary);

  // trades  
  gbox = new QGroupBox;
  gbox->setTitle(tr("Trades"));
  _vbox->insertWidget(pos++, gbox);

  vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  gbox->setLayout(vbox);

  l.clear();
  l << tr("Type") << tr("Volume") << tr("Enter Date") << tr("Enter Price") << tr("Exit Date");
  l << tr("Exit Price") << tr("Bars") << tr("Profit") << tr("Signal") << tr("Equity");

  _tradeList = new QTreeWidget;
  _tradeList->setSortingEnabled(TRUE);
  _tradeList->setRootIsDecorated(FALSE);
  _tradeList->setHeaderLabels(l);
  _tradeList->setSelectionMode(QAbstractItemView::SingleSelection);
  vbox->addWidget(_tradeList);

  _message->hide();
}

void TestDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_localSettings);

  int loop = 0;
  QString key = "test_dialog_summary_col_width_";
  for (; loop < _summary->columnCount(); loop++)
  {
    QString s = key + QString::number(loop);
    _summary->setColumnWidth(loop, settings.value(s, 75).toInt());
  }

  key = "test_dialog_trades_col_width_";
  for (loop = 0; loop < _tradeList->columnCount(); loop++)
  {
    QString s = key + QString::number(loop);
    _tradeList->setColumnWidth(loop, settings.value(s, 75).toInt());
  }
}

void TestDialog::saveSettings ()
{
  Dialog::saveSettings();

  QSettings settings(g_localSettings);

  int loop = 0;
  QString key = "test_dialog_summary_col_width_";
  for (; loop < _summary->columnCount(); loop++)
  {
    QString s = key + QString::number(loop);
    settings.setValue(s, _summary->columnWidth(loop));
  }
  
  key = "test_dialog_trades_col_width_";
  for (loop = 0; loop < _tradeList->columnCount(); loop++)
  {
    QString s = key + QString::number(loop);
    settings.setValue(s, _tradeList->columnWidth(loop));
  }

  settings.sync();
}

void TestDialog::updateTrades ()
{
  _tradeList->clear();

  QList<QTreeWidgetItem *> l = _summary->selectedItems();
  if (! l.count())
    return;
  QTreeWidgetItem *item = l.at(0);
  
  TestDataBase db;
  Setting set;
  set.setData("NAME", item->text(0));

  QList<Setting> trades;
  if (db.trades(set, trades))
  {
    qDebug() << "TestDialog::updateTrades: db error loading trades";
    return;
  }

  int loop = 0;
  for (; loop < trades.count(); loop++)
  {
    Setting trade = trades.at(loop);
    
    QStringList l;
    l << trade.data("TYPE");
    l << trade.data("VOLUME");
    l << trade.data("ENTER_DATE");
    l << trade.data("ENTER_PRICE");
    l << trade.data("EXIT_DATE");
    l << trade.data("EXIT_PRICE");
    l << trade.data("BARS_HELD");
    l << trade.data("PROFIT");
    l << trade.data("SIGNAL");
    l << trade.data("EQUITY");
    new QTreeWidgetItem(_tradeList, l);
  }
}

void TestDialog::updateSummary ()
{
  _summary->clear();
  _tradeList->clear();

  this->setEnabled(FALSE);
  qApp->processEvents();

  qRegisterMetaType<Setting>("Setting");
  SummaryThread *thread = new SummaryThread(this);
  connect(thread, SIGNAL(signalDone()), this, SLOT(updateSummary2()));
  connect(thread, SIGNAL(signalAdd(Setting)), this, SLOT(addSummary(Setting)));
  thread->start();
}

void TestDialog::updateSummary2 ()
{
  this->setEnabled(TRUE);
}

void TestDialog::addSummary (Setting report)
{
  QStringList l;
  l << report.data("NAME");
  l << report.data("TAG");
  l << report.data("SYMBOL");
  l << report.data("EQUITY_GAIN");
  l << report.data("EQUITY");
  l << report.data("TOTAL_PROFIT");
  l << report.data("PROFITABLE_TRADES");
  l << report.data("TOTAL_TRADES");
  l << report.data("WIN_TRADES");
  l << report.data("LOSE_TRADES");
  l << report.data("MAX_DRAWDOWN");
  l << report.data("AVG_PROFIT_LOSS");
  l << report.data("TOTAL_WIN_TRADES");
  l << report.data("TOTAL_LOSE_TRADES");
  l << report.data("AVG_BARS_HELD");
  l << report.data("MIN_BARS_HELD");
  l << report.data("MAX_BARS_HELD");
  l << report.data("TOTAL_COMMISSIONS");
  new QTreeWidgetItem(_summary, l);
}

void TestDialog::summarySelected (QTreeWidgetItem *i)
{
  if (! i)
    return;
  
  updateTrades();
}

void TestDialog::deleteTest ()
{
/*  
  QList<QTreeWidgetItem *> l = _summary->selectedItems();
  if (! l.count())
    return;
  QTreeWidgetItem *item = l.at(0);

  this->setEnabled(FALSE);
  qApp->processEvents();

  DeleteTestThread *thread = new DeleteTestThread(this, item->text(0));
  connect(thread, SIGNAL(signalDone()), this, SLOT(deleteTest2()));
  thread->start();
*/
}

void TestDialog::deleteTest2 ()
{
  updateSummary();
  this->setEnabled(TRUE);
}
