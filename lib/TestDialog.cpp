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
#include "DataDataBase.h"
#include "SummaryThread.h"
#include "TestRuleWidget.h"
#include "NewDialog.h"

#include "../../../pics/refresh.xpm"

#include <QtDebug>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QApplication>
#include <QInputDialog>

TestDialog::TestDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "test_dialog_window_size";
  _keyPos = "test_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Tester");
  setWindowTitle(l.join(" "));

  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  _message->hide();

  createRulePage();
  createSettingsPage();
  createSummaryPage();

  // hide ok button
  _okButton->hide();
  
  // add buttons
  QPushButton *b = _buttonBox->addButton(QString("&Open"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(loadRule()));
  
  b = _buttonBox->addButton(QString("&Save"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(saveRule()));

  b = _buttonBox->addButton(QString("&New"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(newRule()));

  b = _buttonBox->addButton(QString("&Run"), QDialogButtonBox::ActionRole);
  connect(b, SIGNAL(clicked(bool)), this, SLOT(run()));

  loadSettings();

//  updateSummary();
}

void TestDialog::createRulePage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(5);
  w->setLayout(grid);

  // enter long rules
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Enter Long Rules"));
  grid->addWidget(gbox, 0, 0);

  QVBoxLayout *vbox = new QVBoxLayout;
  gbox->setLayout(vbox);

  _enterLongRules = new TestRuleWidget;
  vbox->addWidget(_enterLongRules);

  // exit long rules
  gbox = new QGroupBox;
  gbox->setTitle(tr("Exit Long Rules"));
  grid->addWidget(gbox, 0, 1);

  vbox = new QVBoxLayout;
  gbox->setLayout(vbox);
  
  _exitLongRules = new TestRuleWidget;
  vbox->addWidget(_exitLongRules);

  // enter short rules
  gbox = new QGroupBox;
  gbox->setTitle(tr("Enter Short Rules"));
  grid->addWidget(gbox, 1, 0);

  vbox = new QVBoxLayout;
  gbox->setLayout(vbox);
  
  _enterShortRules = new TestRuleWidget;
  vbox->addWidget(_enterShortRules);

  // exit short rules
  gbox = new QGroupBox;
  gbox->setTitle(tr("Exit Short Rules"));
  grid->addWidget(gbox, 1, 1);

  vbox = new QVBoxLayout;
  gbox->setLayout(vbox);

  _exitShortRules = new TestRuleWidget;
  vbox->addWidget(_exitShortRules);

  _tabs->addTab(w, tr("Rules"));
}

void TestDialog::createSettingsPage ()
{
  QWidget *w = new QWidget;

  QHBoxLayout *hbox = new QHBoxLayout;
  w->setLayout(hbox);

  // left side
  QFormLayout *form = new QFormLayout;
  hbox->addLayout(form);

  // long trades check
  _long = new QCheckBox;
  form->addRow(tr("Long Trades"), _long);

  // short trades check
  _short = new QCheckBox;
  form->addRow(tr("Short Trades"), _short);

  // equity
  _equity = new QDoubleSpinBox;
  _equity->setRange(0, 99999999);
  _equity->setValue(10000);
  form->addRow(tr("Equity"), _equity);

  // enter comm
  _enterComm = new QDoubleSpinBox;
  _enterComm->setRange(0, 999999);
  _enterComm->setValue(10);
  form->addRow(tr("Enter Commision"), _enterComm);

  // exit comm
  _exitComm = new QDoubleSpinBox;
  _exitComm->setRange(0, 999999);
  _exitComm->setValue(10);
  form->addRow(tr("Exit Commision"), _exitComm);

  // volume
  _volume = new QDoubleSpinBox;
  _volume->setRange(0, 1);
  _volume->setValue(0.1);
  form->addRow(tr("Volume %"), _volume);

  // trade delay
  _delay = new QCheckBox;
  form->addRow(tr("Trade Delay"), _delay);

  // symbols
  _symbols = new SymbolButton(this);
  form->addRow(tr("Symbols"), _symbols);

  // right side
  QVBoxLayout *vbox = new QVBoxLayout;
  hbox->addLayout(vbox);

  // max loss stop
  _maxLossStop = new QGroupBox;
  _maxLossStop->setCheckable(TRUE);
  _maxLossStop->setTitle(tr("Maximum Loss Stop"));
  vbox->addWidget(_maxLossStop);

  QFormLayout *tform = new QFormLayout;
  _maxLossStop->setLayout(tform);

  _maxLoss = new QDoubleSpinBox;
  _maxLoss->setRange(0, 1);
  _maxLoss->setValue(0.06);
  tform->addRow(tr("Value"), _maxLoss);
  
  // trailing stop
  _trailingStop = new QGroupBox;
  _trailingStop->setCheckable(TRUE);
  _trailingStop->setTitle(tr("Trailing Stop"));
  vbox->addWidget(_trailingStop);

  tform = new QFormLayout;
  _trailingStop->setLayout(tform);

  _trailing = new QDoubleSpinBox;
  _trailing->setRange(0, 1);
  _trailing->setValue(0.06);
  tform->addRow(tr("Value"), _trailing);

  _tabs->addTab(w, tr("Settings"));
}

void TestDialog::createSummaryPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  w->setLayout(vbox);

  QToolBar *tb = new QToolBar(this);
  vbox->addWidget(tb);

  QToolButton *b = new QToolButton(this);
  b->setIcon(QIcon(refresh_xpm));
  b->setToolTip(tr("Refresh Summary"));
  b->setStatusTip(tr("Refresh Summary"));
  connect(b, SIGNAL(clicked()), this, SLOT(updateSummary()));
  tb->addWidget(b);
  
  // summary
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Summary"));
  vbox->addWidget(gbox);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

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
  tvbox->addWidget(_summary);

  // trades  
  gbox = new QGroupBox;
  gbox->setTitle(tr("Trades"));
  vbox->addWidget(gbox);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  l.clear();
  l << tr("Type") << tr("Volume") << tr("Enter Date") << tr("Enter Price") << tr("Exit Date");
  l << tr("Exit Price") << tr("Bars") << tr("Profit") << tr("Signal") << tr("Equity");

  _tradeList = new QTreeWidget;
  _tradeList->setSortingEnabled(TRUE);
  _tradeList->setRootIsDecorated(FALSE);
  _tradeList->setHeaderLabels(l);
  _tradeList->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_tradeList);

  _tabs->addTab(w, tr("Summary"));
}

void TestDialog::clear ()
{
  _enterLongRules->clear();
  _exitLongRules->clear();
  _enterShortRules->clear();
  _exitShortRules->clear();
  
  _long->setChecked(FALSE);
  _short->setChecked(FALSE);
  _equity->setValue(10000);
  _enterComm->setValue(10);
  _exitComm->setValue(10);
  _volume->setValue(0.1);
  _delay->setChecked(FALSE);
  _symbols->setSymbols(QStringList());
  _maxLossStop->setChecked(FALSE);
  _maxLoss->setValue(0.06);
  _trailingStop->setChecked(FALSE);
  _trailing->setValue(0.06);
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
/*  
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
*/
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

void TestDialog::loadRule ()
{
  DataDataBase db("testRules");
  QStringList l;
  if (db.names(l))
  {
    qDebug() << "TestDialog::loadRule: error loading names";
    return;
  }

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setComboBoxItems(l);
  dialog->setLabelText(tr("Select rule"));
  connect(dialog, SIGNAL(textValueSelected(const QString &)), this, SLOT(loadRule2(QString)));
  dialog->show();
}

void TestDialog::loadRule2 (QString name)
{
  clear();
  _name = name;
  
  _enterLongRules->load(_name, "ENTER_LONG");
  _exitLongRules->load(_name, "EXIT_LONG");
  _enterShortRules->load(_name, "ENTER_SHORT");
  _exitShortRules->load(_name, "EXIT_SHORT");

  Setting set;
  DataDataBase db("testRules");
  if (db.load(_name, &set))
    qDebug() << "TestDialog::loadRule2: error loading rule";

  bool b = set.getInt("LONG");
  _long->setChecked(b);
  
  b = set.getInt("SHORT");
  _short->setChecked(b);
  
  b = set.getInt("DELAY");
  _delay->setChecked(b);
  
  _equity->setValue(set.getDouble("EQUITY"));
  _enterComm->setValue(set.getDouble("ENTER_COMM"));
  _exitComm->setValue(set.getDouble("EXIT_COMM"));
  _maxLoss->setValue(set.getDouble("MAX_LOSS"));
  _trailing->setValue(set.getDouble("TRAILING"));
  _volume->setValue(set.getDouble("VOLUME"));
  _symbols->setSymbols(set.data("SYMBOLS").split(",", QString::SkipEmptyParts));
  
  b = set.getInt("MAX_LOSS_STOP");
  _maxLossStop->setChecked(b);
  
  b = set.getInt("TRAILING_STOP");
  _trailingStop->setChecked(b);
}

void TestDialog::saveRule ()
{
  if (_name.isEmpty())
    return;

  // before saving we need to delete the old one first
  DataDataBase db("testRules");
  db.transaction();
  db.removeName(_name);
  db.commit();

  _enterLongRules->save(_name, "ENTER_LONG");
  _exitLongRules->save(_name, "EXIT_LONG");
  _enterShortRules->save(_name, "ENTER_SHORT");
  _exitShortRules->save(_name, "EXIT_SHORT");

  Setting set;
  set.setData("LONG", _long->isChecked());
  set.setData("SHORT", _short->isChecked());
  set.setData("DELAY", _delay->isChecked());
  set.setData("EQUITY", _equity->text());
  set.setData("ENTER_COMM", _enterComm->text());
  set.setData("EXIT_COMM", _exitComm->text());
  set.setData("MAX_LOSS", _maxLoss->text());
  set.setData("TRAILING", _trailing->text());
  set.setData("VOLUME", _volume->text());
  set.setData("SYMBOLS", _symbols->symbols().join(","));
  set.setData("MAX_LOSS_STOP", _maxLossStop->isChecked());
  set.setData("TRAILING_STOP", _trailingStop->isChecked());

  db.transaction();
  if (db.save(_name, &set))
    qDebug() << "TestDialog::saveRule: error saving rule";
  db.commit();
}

void TestDialog::newRule ()
{
  DataDataBase db("testRules");
  QStringList l;
  if (db.names(l))
  {
    qDebug() << "TestDialog::newRule: error loading names";
    return;
  }

  NewDialog *dialog = new NewDialog(this);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter new rule"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(newRule2(QString)));
  dialog->show();
}

void TestDialog::newRule2 (QString name)
{
  _name = name;
  clear();

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Tester") << "-" << _name;
  setWindowTitle(l.join(" "));
}

void TestDialog::run ()
{
}
