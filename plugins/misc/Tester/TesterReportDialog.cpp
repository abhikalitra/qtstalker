/*
 *  Qtstalker stock charter
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

#include "TesterReportDialog.h"
#include "TesterReportDataBase.h"
#include "TesterConfig.h"
#include "Globals.h"

#include <QLayout>
#include <QDebug>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QPushButton>

TesterReportDialog::TesterReportDialog (QString name)
{
  _name = name;
  
  QString s = "Qtstalker" + g_session + ": " + tr("Tester Report") + " - " + name;
  setWindowTitle(s);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(5);
  vbox->setMargin(5);
  setLayout(vbox);

  QStringList l;
  l << tr("Symbol") << tr("Type") << tr("Volume") << tr("Enter Date") << tr("Enter Price") << tr("Exit Date");
  l << tr("Exit Price") << tr("Profit") << tr("Signal") << tr("Equity");

  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Trades"));
  vbox->addWidget(gbox);

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
  vbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _report = new QTextEdit;
  _report->setReadOnly(TRUE);
  tvbox->addWidget(_report);

  // buttonbox
  QDialogButtonBox *bbox = new QDialogButtonBox;
  vbox->addWidget(bbox);

  QPushButton *b = bbox->addButton(QDialogButtonBox::Close);
  connect(b, SIGNAL(clicked()), this, SLOT(accept()));

  loadSettings();

  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));
}

TesterReportDialog::~TesterReportDialog ()
{
  saveSettings();
}

void TesterReportDialog::loadSettings ()
{
  TesterConfig config;
  
  // restore the size of the window
  QString k = "size" + _name;
  QSize sz;
  config.getData(k, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  k = "position" + _name;
  QPoint p;
  config.getData(k, p);
  if (! p.isNull())
    move(p);

  TesterReportDataBase db;
  TesterReport report;
  report.setName(_name);
  db.getReport(report);
  
  updateTrades(report);

  updateReport(report);
}

void TesterReportDialog::saveSettings ()
{
  TesterConfig config;
  config.transaction();

  // restore the size of the window
  QString k = "size" + _name;
  QSize sz = size();
  config.setData(k, sz);

  // restore the position of the window
  k = "position" + _name;
  QPoint pt = pos();
  config.setData(k, pt);

  config.commit();
}

void TesterReportDialog::updateTrades (TesterReport &report)
{
  _tradeList->clear();

  int loop = 0;
  QStringList trades = report.trades();
  for (; loop < trades.count(); loop++)
  {
    QStringList l = trades.at(loop).split(",");
//    if (l.count() != _tradeList->columnCount())
//      continue;

    new QTreeWidgetItem(_tradeList, l);
  }

  for (loop = 0; loop < _tradeList->columnCount(); loop++)
    _tradeList->resizeColumnToContents(loop);
}

void TesterReportDialog::updateReport (TesterReport &report)
{
  _report->clear();

  _report->append(tr("Profit Factor") + ": \t" + QString::number(report.profitFactor(), 'f', 2));
  _report->append(tr("Payoff Ratio") + ": \t" + QString::number(report.payoffRatio(), 'f', 2));
  _report->append(tr("% Equity Gain") + ": \t" + QString::number(report.equityGain(), 'f', 2));
  _report->append(tr("Equity") + ": \t\t" + QString::number(report.equity(), 'f', 2));
  _report->append(tr("Total Profit") + ": \t" + QString::number(report.totalProfit(), 'f', 2));
  _report->append(tr("% Profitable Trades") + ": \t" + QString::number(report.profitableTrades(), 'f', 2));
  _report->append(tr("Total Trades") + ": \t" + QString::number(report.totalTrades()));
  _report->append(tr("Winning Trades") + ": \t" + QString::number(report.winTrades()));
  _report->append(tr("Losing Trades") + ": \t" + QString::number(report.loseTrades()));
  _report->append(tr("Maximum Drawdown") + ": \t" + QString::number(report.maxDrawdown(), 'f', 2));
  _report->append(tr("Avg Profit/Loss") + ": \t" + QString::number(report.avgProfitLoss(), 'f', 2));
  _report->append(tr("Total Winning Trades") + ": \t" + QString::number(report.totalWinTrades(), 'f', 2));
  _report->append(tr("Total Losing Trades") + ": \t" + QString::number(report.totalLoseTrades(), 'f', 2));
  _report->append(tr("Avg Bars Held") + ": \t" + QString::number(report.avgBarsHeld(), 'f', 2));
  _report->append(tr("Min Bars Held") + ": \t" + QString::number(report.minBarsHeld()));
  _report->append(tr("Max Bars Held") + ": \t" + QString::number(report.maxBarsHeld()));
  _report->append(tr("Total Commissions") + ": \t" + QString::number(report.totalCommissions(), 'f', 2));
}

