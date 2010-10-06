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

#include "TesterSettingsDialog.h"
#include "TesterDataBase.h"
#include "BarData.h"
#include "DateRange.h"
#include "IndicatorPluginFactory.h"
#include "IndicatorPluginDialog.h"
#include "Config.h"
#include "Operator.h"
#include "../../../pics/indicator.xpm"

#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QToolButton>

TesterSettingsDialog::TesterSettingsDialog (QString name)
{
  _saveFlag = FALSE;
  _runningFlag = 0;
  _thread = 0;
  _settings.setName(name);

  QString s = "Qtstalker: " + tr("Tester") + " - " + name;
  setWindowTitle(s);

  QPushButton *b = _buttonBox->button(QDialogButtonBox::Cancel);
  _buttonBox->removeButton(b);
  
  b = _buttonBox->button(QDialogButtonBox::Ok);
  _buttonBox->removeButton(b);

  _saveButton = new QPushButton;
  _buttonBox->addButton(_saveButton, QDialogButtonBox::ActionRole);
  _saveButton->setText(tr("Save"));
  connect(_saveButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
  _saveButton->setEnabled(FALSE);

  _runButton = new QPushButton;
  _buttonBox->addButton(_runButton, QDialogButtonBox::ActionRole);
  _runButton->setText(tr("Run"));
  connect(_runButton, SIGNAL(clicked()), this, SLOT(run()));

  _closeButton = _buttonBox->addButton(QDialogButtonBox::Close);
  connect(_closeButton, SIGNAL(clicked()), this, SLOT(closeDialog()));

  createGeneralPage ();
  createTradesPage ();
  createStopsPage ();
  createIndicatorPage ();
  createReportPage ();

  loadSettings();
}

void TesterSettingsDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  w->setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // initial equity
  QLabel *label = new QLabel(tr("Initial Equity"));
  grid->addWidget(label, row, col++);

  _equity = new QDoubleSpinBox;
  _equity->setRange(0, 999999999);
  connect(_equity, SIGNAL(valueChanged(double)), this, SLOT(equityChanged()));
  grid->addWidget(_equity, row++, col--);
  
  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  QStringList l;
  BarData bd;
  bd.getBarLengthList(l);

  _period = new QComboBox;
  _period->addItems(l);
  connect(_period, SIGNAL(activated(int)), this, SLOT(periodChanged()));
  grid->addWidget(_period, row++, col--);

  // bar range
  label = new QLabel(tr("Date Range"));
  grid->addWidget(label, row, col++);

  DateRange dr;
  l.clear();
  dr.list(l);

  _dateRange = new QComboBox;
  _dateRange->addItems(l);
  _dateRange->setToolTip(tr("The amount of bars to use for the indicator"));
  connect(_dateRange, SIGNAL(activated(int)), this, SLOT(dateRangeChanged()));
  grid->addWidget(_dateRange, row++, col--);
  

  // position size
  label = new QLabel(tr("Position Size"));
  grid->addWidget(label, row, col++);

  _positionSize = new QDoubleSpinBox;
  _positionSize->setRange(0.01, 1.0);
  connect(_positionSize, SIGNAL(valueChanged(double)), this, SLOT(positionSizeChanged()));
  grid->addWidget(_positionSize, row++, col--);

  // commission
  label = new QLabel(tr("Commission"));
  grid->addWidget(label, row, col++);

  l.clear();
  l << tr("Percent") << tr("$ Per Trade") << tr("$ Per Share");
  
  _commission = new QComboBox;
  _commission->addItems(l);
  connect(_commission, SIGNAL(activated(int)), this, SLOT(commissionChanged()));
  grid->addWidget(_commission, row, col++);

  _commissionValue = new QDoubleSpinBox;
  _commissionValue->setRange(0.01, 9999);
  connect(_commissionValue, SIGNAL(valueChanged(double)), this, SLOT(commissionValueChanged()));
  grid->addWidget(_commissionValue, row++, col);
  col -= 2;

  // symbols
  label = new QLabel(tr("Symbols"));
  grid->addWidget(label, row, col++);

  _symbols = new SymbolButton(w);
  connect(_symbols, SIGNAL(signalChanged()), this, SLOT(symbolsChanged()));
  grid->addWidget(_symbols, row++, col--);
  
  // futures mode
  label = new QLabel(tr("Futures Mode"));
  grid->addWidget(label, row, col++);

  _futuresMode = new QCheckBox;
  connect(_futuresMode, SIGNAL(stateChanged(int)), this, SLOT(futuresModeChanged()));
  grid->addWidget(_futuresMode, row++, col--);


  vbox->addStretch();

  _tabs->addTab(w, tr("General"));
}

void TesterSettingsDialog::createTradesPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // long trades
  _long = new QCheckBox(tr("Long"));
  connect(_long, SIGNAL(stateChanged(int)), this, SLOT(longChanged(int)));
  grid->addWidget(_long, row, col++);
  
  QStringList l;
  BarData bd;
  bd.getInputFields(l);

  _longBuyPrice = new QComboBox;
  _longBuyPrice->addItems(l);
  connect(_longBuyPrice, SIGNAL(activated(int)), this, SLOT(longBuyPriceChanged()));
  grid->addWidget(_longBuyPrice, row, col++);

  _longSellPrice = new QComboBox;
  _longSellPrice->addItems(l);
  connect(_longSellPrice, SIGNAL(activated(int)), this, SLOT(longSellPriceChanged()));
  grid->addWidget(_longSellPrice, row++, col);
  col -= 2;

  // short trades
  _short = new QCheckBox(tr("Short"));
  connect(_short, SIGNAL(stateChanged(int)), this, SLOT(shortChanged(int)));
  grid->addWidget(_short, row, col++);

  _shortBuyPrice = new QComboBox;
  _shortBuyPrice->addItems(l);
  connect(_shortBuyPrice, SIGNAL(activated(int)), this, SLOT(shortBuyPriceChanged()));
  grid->addWidget(_shortBuyPrice, row, col++);

  _shortSellPrice = new QComboBox;
  _shortSellPrice->addItems(l);
  connect(_shortSellPrice, SIGNAL(activated(int)), this, SLOT(shortSellPriceChanged()));
  grid->addWidget(_shortSellPrice, row++, col);
  col -= 2;

  vbox->addStretch();

  longChanged(0);
  shortChanged(0);

  _tabs->addTab(w, tr("Trades"));
}

void TesterSettingsDialog::createStopsPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(4, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // maximum loss
  _maximumLossStop = new QCheckBox(tr("Maximum Loss"));
  connect(_maximumLossStop, SIGNAL(stateChanged(int)), this, SLOT(maximumLossStopChanged(int)));
  grid->addWidget(_maximumLossStop, row, col++);

  QStringList l;
  l << tr("Percent") << tr("Point");

  _maximumLossType = new QComboBox;
  _maximumLossType->addItems(l);
  connect(_maximumLossType, SIGNAL(activated(int)), this, SLOT(maximumLossTypeChanged()));
  grid->addWidget(_maximumLossType, row, col++);

  _maximumLossValue = new QDoubleSpinBox;
  _maximumLossValue->setRange(0.01, 99999999);
  connect(_maximumLossValue, SIGNAL(valueChanged(double)), this, SLOT(maximumLossValueChanged()));
  grid->addWidget(_maximumLossValue, row, col++);

  QStringList l2;
  BarData bd;
  bd.getInputFields(l2);

  _maximumLossExit = new QComboBox;
  _maximumLossExit->addItems(l2);
  connect(_maximumLossExit, SIGNAL(activated(int)), this, SLOT(maximumLossExitChanged()));
  grid->addWidget(_maximumLossExit, row++, col);
  col -= 3;

  // profit target
  _profitTargetStop = new QCheckBox(tr("Profit Target"));
  connect(_profitTargetStop, SIGNAL(stateChanged(int)), this, SLOT(profitTargetStopChanged(int)));
  grid->addWidget(_profitTargetStop, row, col++);

  _profitTargetType = new QComboBox;
  _profitTargetType->addItems(l);
  connect(_profitTargetType, SIGNAL(activated(int)), this, SLOT(profitTargetTypeChanged()));
  grid->addWidget(_profitTargetType, row, col++);

  _profitTargetValue = new QDoubleSpinBox;
  _profitTargetValue->setRange(0.01, 99999999);
  connect(_profitTargetValue, SIGNAL(valueChanged(double)), this, SLOT(profitTargetValueChanged()));
  grid->addWidget(_profitTargetValue, row, col++);

  _profitTargetExit = new QComboBox;
  _profitTargetExit->addItems(l2);
  connect(_profitTargetExit, SIGNAL(activated(int)), this, SLOT(profitTargetExitChanged()));
  grid->addWidget(_profitTargetExit, row++, col);
  col -= 3;

  // trailing stop
  _trailingStop = new QCheckBox(tr("Trailing"));
  connect(_trailingStop, SIGNAL(stateChanged(int)), this, SLOT(trailingStopChanged(int)));
  grid->addWidget(_trailingStop, row, col++);

  _trailingType = new QComboBox;
  _trailingType->addItems(l);
  connect(_trailingType, SIGNAL(activated(int)), this, SLOT(trailingTypeChanged()));
  grid->addWidget(_trailingType, row, col++);

  _trailingValue = new QDoubleSpinBox;
  _trailingValue->setRange(0.01, 99999999);
  connect(_trailingValue, SIGNAL(valueChanged(double)), this, SLOT(trailingValueChanged()));
  grid->addWidget(_trailingValue, row, col++);

  _trailingExit = new QComboBox;
  _trailingExit->addItems(l2);
  connect(_trailingExit, SIGNAL(activated(int)), this, SLOT(trailingExitChanged()));
  grid->addWidget(_trailingExit, row++, col);
  col -= 3;

  // bars stop
  _barsStop = new QCheckBox(tr("Bars"));
  connect(_barsStop, SIGNAL(stateChanged(int)), this, SLOT(barsStopChanged(int)));
  grid->addWidget(_barsStop, row, col++);

  _barsStopValue = new QSpinBox;
  _barsStopValue->setRange(1, 99999999);
  connect(_barsStopValue, SIGNAL(valueChanged(int)), this, SLOT(barsStopValueChanged()));
  grid->addWidget(_barsStopValue, row, col++);

  _barsStopExit = new QComboBox;
  _barsStopExit->addItems(l2);
  connect(_barsStopExit, SIGNAL(activated(int)), this, SLOT(barsStopExitChanged()));
  grid->addWidget(_barsStopExit, row++, col);
  col -= 2;

  vbox->addStretch();

  maximumLossStopChanged(0);
  profitTargetStopChanged(0);
  trailingStopChanged(0);
  barsStopChanged(0);
  
  _tabs->addTab(w, tr("Stops"));
}

void TesterSettingsDialog::createIndicatorPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(10);
  w->setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(10);
  grid->setColumnStretch(3, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // indicator
  QLabel *label = new QLabel(tr("Indicator"));
  grid->addWidget(label, row, col++);

  Config config;
  QStringList l;
  config.getData(Config::IndicatorPluginList, l);

  _indicator = new QComboBox;
  _indicator->addItems(l);
  _indicator->setCurrentIndex(-1);
  connect(_indicator, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  connect(_indicator, SIGNAL(activated(int)), this, SLOT(indicatorChanged(int)));
  grid->addWidget(_indicator, row, col++);

  QToolButton *b = new QToolButton;
  b->setToolTip(tr("Indicator Settings..."));
  b->setIcon(QPixmap(indicator_xpm));
  connect(b, SIGNAL(clicked()), this, SLOT(editIndicator()));
  grid->addWidget(b, row++, col);
  col -= 2;

  // indicator logic
  l.clear();
  l << tr("Enable") << tr("Plot") << tr("Operator") << tr("Value");

  QTabWidget *tabs = new QTabWidget;
  vbox->addWidget(tabs);

  // enter long tab
  QWidget *tw = new QWidget;

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  vbox->setSpacing(0);
  tw->setLayout(tvbox);

  _enterLong = new QTreeWidget;
  _enterLong->setSortingEnabled(FALSE);
  _enterLong->setRootIsDecorated(FALSE);
  _enterLong->setHeaderLabels(l);
  _enterLong->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_enterLong);

  tvbox->addStretch();

  tabs->addTab(tw, tr("Enter Long"));
  
  // exit long tab
  tw = new QWidget;

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  vbox->setSpacing(0);
  tw->setLayout(tvbox);
  
  _exitLong = new QTreeWidget;
  _exitLong->setSortingEnabled(FALSE);
  _exitLong->setRootIsDecorated(FALSE);
  _exitLong->setHeaderLabels(l);
  _exitLong->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_exitLong);

  tvbox->addStretch();

  tabs->addTab(tw, tr("Exit Long"));

  // enter short tab
  tw = new QWidget;

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  vbox->setSpacing(0);
  tw->setLayout(tvbox);

  _enterShort = new QTreeWidget;
  _enterShort->setSortingEnabled(FALSE);
  _enterShort->setRootIsDecorated(FALSE);
  _enterShort->setHeaderLabels(l);
  _enterShort->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_enterShort);

  tvbox->addStretch();

  tabs->addTab(tw, tr("Enter Short"));

  // exit short tab
  tw = new QWidget;

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  vbox->setSpacing(0);
  tw->setLayout(tvbox);

  _exitShort = new QTreeWidget;
  _exitShort->setSortingEnabled(FALSE);
  _exitShort->setRootIsDecorated(FALSE);
  _exitShort->setHeaderLabels(l);
  _exitShort->setSelectionMode(QAbstractItemView::SingleSelection);
  tvbox->addWidget(_exitShort);

  tvbox->addStretch();

  tabs->addTab(tw, tr("Exit Short"));

  
  vbox->addStretch();

  _tabs->addTab(w, tr("Indicator"));
}

void TesterSettingsDialog::createReportPage ()
{
  QWidget *w = new QWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(2);
  w->setLayout(vbox);

  QStringList l;
  l << tr("Type") << tr("Volume") << tr("Enter Date") << tr("Enter Price") << tr("Exit Date");
  l << tr("Exit Price") << tr("Profit") << tr("Signal");
  
  _tradeList = new QTreeWidget;
  _tradeList->setSortingEnabled(FALSE);
  _tradeList->setRootIsDecorated(FALSE);
  _tradeList->setHeaderLabels(l);
  _tradeList->setSelectionMode(QAbstractItemView::SingleSelection);
  vbox->addWidget(_tradeList);

  _report = new QTextEdit;
  _report->setReadOnly(TRUE);
  vbox->addWidget(_report);

  _tabs->addTab(w, tr("Report"));
}

void TesterSettingsDialog::loadSettings ()
{
  TesterDataBase db;
  db.getRule(_settings);

  _equity->setValue(_settings.equity());
  _period->setCurrentIndex(_settings.period());
  _dateRange->setCurrentIndex(_settings.dateRange());
  _positionSize->setValue(_settings.positionSize());
  _futuresMode->setChecked(_settings.futuresMode());
  _commission->setCurrentIndex(_settings.commission());
  _commissionValue->setValue(_settings.commissionValue());
  _long->setCheckState((Qt::CheckState) _settings.getLong());
  _longBuyPrice->setCurrentIndex(_settings.longBuyPrice());
  _longSellPrice->setCurrentIndex(_settings.longSellPrice());
  _short->setCheckState((Qt::CheckState) _settings.getShort());
  _shortBuyPrice->setCurrentIndex(_settings.shortBuyPrice());
  _shortSellPrice->setCurrentIndex(_settings.shortSellPrice());
  _maximumLossStop->setCheckState((Qt::CheckState) _settings.maximumLossStop());
  _maximumLossType->setCurrentIndex(_settings.maximumLossType());
  _maximumLossValue->setValue(_settings.maximumLossValue());
  _maximumLossExit->setCurrentIndex(_settings.maximumLossExit());
  _profitTargetStop->setCheckState((Qt::CheckState) _settings.profitTargetStop());
  _profitTargetType->setCurrentIndex(_settings.profitTargetType());
  _profitTargetValue->setValue(_settings.profitTargetValue());
  _profitTargetExit->setCurrentIndex(_settings.profitTargetExit());
  _trailingStop->setCheckState((Qt::CheckState) _settings.trailingStop());
  _trailingType->setCurrentIndex(_settings.trailingType());
  _trailingValue->setValue(_settings.trailingValue());
  _trailingExit->setCurrentIndex(_settings.trailingExit());
  _barsStop->setCheckState((Qt::CheckState) _settings.barsStop());
  _barsStopValue->setValue(_settings.barsStopValue());
  _barsStopExit->setCurrentIndex(_settings.barsStopExit());
  _indicator->setCurrentIndex(_indicator->findText(_settings.indicator()));

  _report->clear();
  _report->append(_settings.report());

  QStringList l = _settings.enterLong();
  if (l.count())
  {
    _enterLong->clear();
    
    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList l2 = l.at(loop).split(",");
      addPlotItem(_enterLong, _settings.plotNames(), l2.at(0), l2.at(1), l2.at(2), l2.at(3));
    }
  }
  
  l = _settings.exitLong();
  if (l.count())
  {
    _exitLong->clear();
    
    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList l2 = l.at(loop).split(",");
      addPlotItem(_exitLong, _settings.plotNames(), l2.at(0), l2.at(1), l2.at(2), l2.at(3));
    }
  }

  l = _settings.enterShort();
  if (l.count())
  {
    _enterShort->clear();
    
    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList l2 = l.at(loop).split(",");
      addPlotItem(_enterShort, _settings.plotNames(), l2.at(0), l2.at(1), l2.at(2), l2.at(3));
    }
  }

  l = _settings.exitShort();
  if (l.count())
  {
    _exitShort->clear();
    
    int loop = 0;
    for (; loop < l.count(); loop++)
    {
      QStringList l2 = l.at(loop).split(",");
      addPlotItem(_exitShort, _settings.plotNames(), l2.at(0), l2.at(1), l2.at(2), l2.at(3));
    }
  }

  _symbols->setSymbolList(_settings.symbols());

  updateTradeList();

  _saveFlag = 0;
  _saveButton->setEnabled(FALSE);
}

void TesterSettingsDialog::saveSettings ()
{
  if (! _saveFlag)
    return;

  TesterDataBase db;
  db.transaction();
  db.setRule(_settings);
  db.commit();

  _saveFlag = 0;
  _saveButton->setEnabled(FALSE);
}

void TesterSettingsDialog::getPlotItems (QTreeWidget *tree, QStringList &plotNames, QStringList &items)
{
  plotNames.clear();
  items.clear();
  
  int loop = 0;
  for (; loop < tree->topLevelItemCount(); loop++)
  {
    QTreeWidgetItem *item = tree->topLevelItem(loop);

    QStringList l;
    QCheckBox *check = (QCheckBox *) tree->itemWidget(item, 0);
    l << QString::number(check->checkState());

    l << item->text(1);
    plotNames.append(item->text(1));

    QComboBox *cb = (QComboBox *) tree->itemWidget(item, 2);
    l << QString::number(cb->currentIndex());

    cb = (QComboBox *) tree->itemWidget(item, 3);
    l << cb->currentText();

    items << l.join(",");
  }
}

void TesterSettingsDialog::barsStopExitChanged ()
{
  _settings.setBarsStopExit(_barsStopExit->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::barsStopValueChanged ()
{
  _settings.setBarsStopValue(_barsStopValue->value());
  ruleChanged();
}

void TesterSettingsDialog::trailingExitChanged ()
{
  _settings.setTrailingExit(_trailingExit->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::trailingValueChanged ()
{
  _settings.setTrailingValue(_trailingValue->value());
  ruleChanged();
}

void TesterSettingsDialog::trailingTypeChanged ()
{
  _settings.setTrailingType(_trailingType->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::profitTargetExitChanged ()
{
  _settings.setProfitTargetExit(_profitTargetExit->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::profitTargetValueChanged ()
{
  _settings.setProfitTargetValue(_profitTargetValue->value());
  ruleChanged();
}

void TesterSettingsDialog::profitTargetTypeChanged ()
{
  _settings.setProfitTargetType(_profitTargetType->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::maximumLossExitChanged ()
{
  _settings.setMaximumLossExit(_maximumLossExit->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::maximumLossValueChanged ()
{
  _settings.setMaximumLossValue(_maximumLossValue->value());
  ruleChanged();
}

void TesterSettingsDialog::maximumLossTypeChanged ()
{
  _settings.setMaximumLossType(_maximumLossType->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::shortSellPriceChanged ()
{
  _settings.setShortSellPrice(_shortSellPrice->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::shortBuyPriceChanged ()
{
  _settings.setShortBuyPrice(_shortBuyPrice->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::longSellPriceChanged ()
{
  _settings.setLongSellPrice(_longSellPrice->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::longBuyPriceChanged ()
{
  _settings.setLongBuyPrice(_longBuyPrice->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::futuresModeChanged ()
{
  _settings.setFuturesMode(_futuresMode->isChecked());
  ruleChanged();
}

void TesterSettingsDialog::symbolsChanged ()
{
  _symbols->symbolList(_settings.symbols());
  ruleChanged();
}

void TesterSettingsDialog::commissionValueChanged ()
{
  _settings.setCommissionValue(_commissionValue->value());
  ruleChanged();
}

void TesterSettingsDialog::commissionChanged ()
{
  _settings.setCommission(_commission->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::positionSizeChanged ()
{
  _settings.setPositionSize(_positionSize->value());
  ruleChanged();
}

void TesterSettingsDialog::dateRangeChanged ()
{
  _settings.setDateRange(_dateRange->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::periodChanged ()
{
  _settings.setPeriod(_period->currentIndex());
  ruleChanged();
}

void TesterSettingsDialog::equityChanged ()
{
  _settings.setEquity(_equity->value());
  ruleChanged();
}

void TesterSettingsDialog::ruleChanged ()
{
  _saveFlag = TRUE;
  _saveButton->setEnabled(_saveFlag);
}

void TesterSettingsDialog::longChanged (int state)
{
  _settings.setLong(state);
  _longBuyPrice->setEnabled(state);
  _longSellPrice->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::shortChanged (int state)
{
  _settings.setShort(state);
  _shortBuyPrice->setEnabled(state);
  _shortSellPrice->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::maximumLossStopChanged (int state)
{
  _settings.setMaximumLossStop(state);
  _maximumLossType->setEnabled(state);
  _maximumLossValue->setEnabled(state);
  _maximumLossExit->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::profitTargetStopChanged (int state)
{
  _settings.setProfitTargetStop(state);
  _profitTargetType->setEnabled(state);
  _profitTargetValue->setEnabled(state);
  _profitTargetExit->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::trailingStopChanged (int state)
{
  _settings.setTrailingStop(state);
  _trailingType->setEnabled(state);
  _trailingValue->setEnabled(state);
  _trailingExit->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::barsStopChanged (int state)
{
  _settings.setBarsStop(state);
  _barsStopValue->setEnabled(state);
  _barsStopExit->setEnabled(state);
  ruleChanged();
}

void TesterSettingsDialog::plotItemChanged ()
{
  getPlotItems(_enterLong, _settings.plotNames(), _settings.enterLong());
  getPlotItems(_exitLong, _settings.plotNames(), _settings.exitLong());
  getPlotItems(_enterShort, _settings.plotNames(), _settings.enterShort());
  getPlotItems(_exitShort, _settings.plotNames(), _settings.exitShort());

  ruleChanged();
}

void TesterSettingsDialog::indicatorChanged (int)
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plugin = fac.plugin(_indicator->currentText());
  if (! plugin)
  {
    qDebug() << "TesterSettingsDialog::indicatorChanged: no plugin";
    return;
  }

  _settings.setIndicator(_indicator->currentText());

  Indicator i;
  plugin->defaults(i);
  _settings.setIndicatorSettings(i.settings());
  
  QStringList plotNames;
  plugin->plotNames(i, plotNames);

  _enterLong->clear();
  _exitLong->clear();
  _enterShort->clear();
  _exitShort->clear();

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
  {
    addPlotItem(_enterLong, plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));
    addPlotItem(_exitLong, plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));
    addPlotItem(_enterShort, plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));
    addPlotItem(_exitShort, plotNames, QString("0"), plotNames.at(loop), QString("0"), QString("Close"));
  }

  plotItemChanged();
}

void TesterSettingsDialog::addPlotItem (QTreeWidget *tree, QStringList &plotNames, QString enable,
                                        QString pn, QString oper, QString val)
{
  QTreeWidgetItem *item = new QTreeWidgetItem(tree);

  QCheckBox *check = new QCheckBox;
  check->setCheckState((Qt::CheckState) enable.toInt());
  tree->setItemWidget(item, 0, check);
  connect(check, SIGNAL(stateChanged(int)), this, SLOT(plotItemChanged()));
  
  item->setText(1, pn);

  QComboBox *cb = new QComboBox;
  Operator op;
  cb->addItems(op.list());
  cb->setCurrentIndex(oper.toInt());
  tree->setItemWidget(item, 2, cb);
  connect(cb, SIGNAL(currentIndexChanged(int)), this, SLOT(plotItemChanged()));

  QComboBox *vc = new QComboBox;
  vc->setEditable(TRUE);
  vc->addItems(plotNames);
  
  QStringList l;
  l << "Open" << "High" << "Low" << "Close" << "Volume" << "OI";
  vc->addItems(l);

  vc->clearEditText();
  vc->setCurrentIndex(vc->findText(val));
  tree->setItemWidget(item, 3, vc);
  connect(vc, SIGNAL(currentIndexChanged(int)), this, SLOT(plotItemChanged()));

  int loop = 0;
  for (; loop < tree->columnCount(); loop++)
    tree->resizeColumnToContents(loop);
}

void TesterSettingsDialog::editIndicator ()
{
  IndicatorPluginFactory fac;
  IndicatorPlugin *plug = fac.plugin(_indicator->currentText());
  if (! plug)
  {
    qDebug() << "TesterSettingsDialog::editIndicator: no plugin";
    return;
  }

  Indicator i;
  if (! _settings.indicatorSettings().count())
  {
    plug->defaults(i);
    _settings.setIndicatorSettings(i.settings());
  }
  else
    i.setSettings(_settings.indicatorSettings());

  IndicatorPluginDialog *dialog = plug->dialog(i);
  if (! dialog)
  {
    qDebug() << "TesterSettingsDialog::editIndicator: no dialog";
    return;
  }

  connect(dialog, SIGNAL(signalDone(Indicator)), this, SLOT(editIndicator2(Indicator)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void TesterSettingsDialog::editIndicator2 (Indicator i)
{
  _settings.setIndicatorSettings(i.settings());
}

void TesterSettingsDialog::closeDialog ()
{
  if (_runningFlag)
    _thread->stop();
  else
  {
    saveSettings();
    accept();
  }
}

void TesterSettingsDialog::done (QString report, QStringList trades)
{
  _runningFlag = 0;
  this->setEnabled(TRUE);

  _closeButton->setText(tr("Close"));

  _report->clear();
  _report->append(report);
  _settings.setReport(report);

  _settings.setTrades(trades);
  updateTradeList();

  ruleChanged();
}

void TesterSettingsDialog::updateTradeList ()
{
  _tradeList->clear();

  int loop = 0;
  QStringList trades = _settings.trades();
  for (; loop < trades.count(); loop++)
  {
    QStringList l = trades.at(loop).split(",");
    if (l.count() != _tradeList->columnCount())
      continue;

    new QTreeWidgetItem(_tradeList, l);
  }

  for (loop = 0; loop < _tradeList->columnCount(); loop++)
    _tradeList->resizeColumnToContents(loop);
}

void TesterSettingsDialog::run ()
{
  _thread = new TesterThread(this, _settings);
  connect(_thread, SIGNAL(signalDone(QString, QStringList)), this, SLOT(done(QString, QStringList)));
  _thread->start();

  _runningFlag = 1;
  
  this->setEnabled(FALSE);
  
  _closeButton->setText(tr("Stop"));
  _closeButton->setEnabled(TRUE);
}

