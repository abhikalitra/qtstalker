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
#include "TesterConfig.h"
#include "Globals.h"

#include <QLayout>
#include <QLabel>
#include <QDebug>
#include <QToolButton>
#include <QFormLayout>
#include <QDialogButtonBox>

TesterSettingsDialog::TesterSettingsDialog (QString name)
{
  _saveFlag = FALSE;
  _settings.setName(name);
//  _helpFile = "Tester.html";

  QString s = "Qtstalker" + g_session + ": " + tr("Tester") + " - " + name;
  setWindowTitle(s);

  _buttonBox->removeButton(_buttonBox->button(QDialogButtonBox::Ok));
  _buttonBox->removeButton(_buttonBox->button(QDialogButtonBox::Cancel));
  
  _saveButton = new QPushButton;
  _buttonBox->addButton(_saveButton, QDialogButtonBox::ActionRole);
  _saveButton->setText(tr("Apply"));
  connect(_saveButton, SIGNAL(clicked()), this, SLOT(saveSettings()));
  _saveButton->setEnabled(FALSE);

  _closeButton = _buttonBox->addButton(QDialogButtonBox::Close);
  connect(_closeButton, SIGNAL(clicked()), this, SLOT(cancel()));

  createGeneralPage ();
  createTradesPage ();
  createStopsPage ();
  createIndicatorPage ();

  loadSettings();
}

void TesterSettingsDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  w->setLayout(form);

  _equity = new QDoubleSpinBox;
  _equity->setRange(0, 999999999);
  connect(_equity, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Initial Equity"), _equity);

  // period
  QStringList l;
  BarData bd;
  bd.getBarLengthList(l);

  _period = new QComboBox;
  _period->addItems(l);
  connect(_period, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Period"), _period);

  // date range
  DateRange dr;
  l.clear();
  dr.list(l);

  _dateRange = new QComboBox;
  _dateRange->addItems(l);
  _dateRange->setToolTip(tr("The amount of bars to use for the indicator"));
  connect(_dateRange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Date Range"), _dateRange);

  // position size
  _positionSize = new QDoubleSpinBox;
  _positionSize->setRange(0.01, 1.0);
  connect(_positionSize, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Position Size"), _positionSize);

  // symbols
  _symbols = new SymbolButton(w);
  connect(_symbols, SIGNAL(signalChanged()), this, SLOT(ruleChanged()));
  form->addRow(tr("Symbols"), _symbols);

  // commission type
  l.clear();
  l << tr("Percent") << tr("$ Per Trade") << tr("$ Per Share");

  _commission = new QComboBox;
  _commission->addItems(l);
  connect(_commission, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Commission Type"), _commission);

  // commission value
  _commissionValue = new QDoubleSpinBox;
  _commissionValue->setRange(0.01, 9999);
  connect(_commissionValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Commission Value"), _commissionValue);

  // futures mode
  _futuresMode = new QCheckBox;
  connect(_futuresMode, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Futures Mode"), _futuresMode);

  _tabs->addTab(w, tr("General"));
}

void TesterSettingsDialog::createTradesPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // long trades
  _long = new QGroupBox;
  _long->setTitle(tr("Long Trades"));
  _long->setCheckable(TRUE);
  connect(_long, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_long, row++, col);

  QFormLayout *form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _long->setLayout(form);

  // long buy price
  QStringList l;
  BarData bd;
  bd.getInputFields(l);

  _longBuyPrice = new QComboBox;
  _longBuyPrice->addItems(l);
  connect(_longBuyPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Buy Price"), _longBuyPrice);

  // long sell price
  _longSellPrice = new QComboBox;
  _longSellPrice->addItems(l);
  connect(_longSellPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Sell Price"), _longSellPrice);

  _short = new QGroupBox;
  _short->setTitle(tr("Short Trades"));
  _short->setCheckable(TRUE);
  connect(_short, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_short, row, col);

  form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _short->setLayout(form);

  // short buy price
  _shortBuyPrice = new QComboBox;
  _shortBuyPrice->addItems(l);
  connect(_shortBuyPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Buy Price"), _shortBuyPrice);

  // short sell price
  _shortSellPrice = new QComboBox;
  _shortSellPrice->addItems(l);
  connect(_shortSellPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Sell Price"), _shortSellPrice);

  _tabs->addTab(w, tr("Trades"));
}

void TesterSettingsDialog::createStopsPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(5);
  grid->setSpacing(5);
  w->setLayout(grid);

  int row = 0;
  int col = 0;
  
  // maximum loss
  _maximumLossStop = new QGroupBox;
  _maximumLossStop->setTitle(tr("Maximum Loss"));
  _maximumLossStop->setCheckable(TRUE);
  connect(_maximumLossStop, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_maximumLossStop, row, col++);

  QFormLayout *form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _maximumLossStop->setLayout(form);

  // maximum loss type
  QStringList l;
  l << tr("Percent") << tr("Point");

  _maximumLossType = new QComboBox;
  _maximumLossType->addItems(l);
  connect(_maximumLossType, SIGNAL(activated(int)), this, SLOT(maximumLossTypeChanged()));
  form->addRow(tr("Type"), _maximumLossType);

  // maximum loss value
  _maximumLossValue = new QDoubleSpinBox;
  _maximumLossValue->setRange(0.01, 99999999);
  connect(_maximumLossValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Value"), _maximumLossValue);

  // maximum loss exit
  QStringList l2;
  BarData bd;
  bd.getInputFields(l2);

  _maximumLossExit = new QComboBox;
  _maximumLossExit->addItems(l2);
  connect(_maximumLossExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Exit"), _maximumLossExit);

  // profit target
  _profitTargetStop = new QGroupBox;
  _profitTargetStop->setTitle(tr("Profit Target"));
  _profitTargetStop->setCheckable(TRUE);
  connect(_profitTargetStop, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_profitTargetStop, row++, col--);

  form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _profitTargetStop->setLayout(form);

  // profit target type
  _profitTargetType = new QComboBox;
  _profitTargetType->addItems(l);
  connect(_profitTargetType, SIGNAL(activated(int)), this, SLOT(profitTargetTypeChanged()));
  form->addRow(tr("Type"), _profitTargetType);

  // profit target value
  _profitTargetValue = new QDoubleSpinBox;
  _profitTargetValue->setRange(0.01, 99999999);
  connect(_profitTargetValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Value"), _profitTargetValue);

  // profit target exit
  _profitTargetExit = new QComboBox;
  _profitTargetExit->addItems(l2);
  connect(_profitTargetExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Exit"), _profitTargetExit);

  // trailing stop
  _trailingStop = new QGroupBox;
  _trailingStop->setTitle(tr("Trailing"));
  _trailingStop->setCheckable(TRUE);
  connect(_trailingStop, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_trailingStop, row, col++);

  form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _trailingStop->setLayout(form);

  // trailing type
  _trailingType = new QComboBox;
  _trailingType->addItems(l);
  connect(_trailingType, SIGNAL(activated(int)), this, SLOT(trailingTypeChanged()));
  form->addRow(tr("Type"), _trailingType);

  // trailing value
  _trailingValue = new QDoubleSpinBox;
  _trailingValue->setRange(0.01, 99999999);
  connect(_trailingValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  form->addRow(tr("Value"), _trailingValue);

  // trailing exit
  _trailingExit = new QComboBox;
  _trailingExit->addItems(l2);
  connect(_trailingExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Exit"), _trailingExit);

  // bars stop
  _barsStop = new QGroupBox;
  _barsStop->setTitle(tr("Bars"));
  _barsStop->setCheckable(TRUE);
  connect(_barsStop, SIGNAL(toggled(bool)), this, SLOT(ruleChanged()));
  grid->addWidget(_barsStop, row++, col--);

  form = new QFormLayout;
  form->setMargin(5);
  form->setSpacing(2);
  _barsStop->setLayout(form);

  // bars stop value
  _barsStopValue = new QSpinBox;
  _barsStopValue->setRange(1, 99999999);
  connect(_barsStopValue, SIGNAL(valueChanged(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Value"), _barsStopValue);

  // bars stop exit
  _barsStopExit = new QComboBox;
  _barsStopExit->addItems(l2);
  connect(_barsStopExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  form->addRow(tr("Exit"), _barsStopExit);

  _tabs->addTab(w, tr("Stops"));
}

void TesterSettingsDialog::createIndicatorPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(5);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // enter long
  QGroupBox *gbox = new QGroupBox;
  gbox->setTitle(tr("Enter Long"));
  grid->addWidget(gbox, row, col++);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _enterLong = new IndicatorPlotList;
  connect(_enterLong, SIGNAL(signalIndicatorChanged()), this, SLOT(ruleChanged()));
  connect(_enterLong, SIGNAL(signalItemChanged()), this, SLOT(ruleChanged()));
  tvbox->addWidget(_enterLong);

  // exit long
  gbox = new QGroupBox;
  gbox->setTitle(tr("Exit Long"));
  grid->addWidget(gbox, row++, col--);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);
  
  _exitLong = new IndicatorPlotList;
  connect(_exitLong, SIGNAL(signalIndicatorChanged()), this, SLOT(ruleChanged()));
  connect(_exitLong, SIGNAL(signalItemChanged()), this, SLOT(ruleChanged()));
  tvbox->addWidget(_exitLong);

  // enter short
  gbox = new QGroupBox;
  gbox->setTitle(tr("Enter Short"));
  grid->addWidget(gbox, row, col++);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _enterShort = new IndicatorPlotList;
  connect(_enterShort, SIGNAL(signalIndicatorChanged()), this, SLOT(ruleChanged()));
  connect(_enterShort, SIGNAL(signalItemChanged()), this, SLOT(ruleChanged()));
  tvbox->addWidget(_enterShort);

  // exit short
  gbox = new QGroupBox;
  gbox->setTitle(tr("Exit Short"));
  grid->addWidget(gbox, row++, col--);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

  _exitShort = new IndicatorPlotList;
  connect(_exitShort, SIGNAL(signalIndicatorChanged()), this, SLOT(ruleChanged()));
  connect(_exitShort, SIGNAL(signalItemChanged()), this, SLOT(ruleChanged()));
  tvbox->addWidget(_exitShort);

  _tabs->addTab(w, tr("Indicator"));
}

void TesterSettingsDialog::loadSettings ()
{
  TesterConfig config;
  
  // restore the size of the window
  QString k = "size" + _settings.name();
  QSize sz;
  config.getData(k, sz);
  if (! sz.isNull())
    resize(sz);

  // restore the position of the window
  k = "position" + _settings.name();
  QPoint p;
  config.getData(k, p);
  if (! p.isNull())
    move(p);
  
  
  TesterDataBase db;
  db.getRule(_settings);

  _equity->setValue(_settings.equity());
  _period->setCurrentIndex(_settings.period());
  _dateRange->setCurrentIndex(_settings.dateRange());
  _positionSize->setValue(_settings.positionSize());
  _futuresMode->setChecked(_settings.futuresMode());
  _commission->setCurrentIndex(_settings.commission());
  _commissionValue->setValue(_settings.commissionValue());
  _long->setChecked((bool) _settings.getLong());
  _longBuyPrice->setCurrentIndex(_settings.longBuyPrice());
  _longSellPrice->setCurrentIndex(_settings.longSellPrice());
  _short->setChecked((bool) _settings.getShort());
  _shortBuyPrice->setCurrentIndex(_settings.shortBuyPrice());
  _shortSellPrice->setCurrentIndex(_settings.shortSellPrice());
  _maximumLossStop->setChecked((bool) _settings.maximumLossStop());
  _maximumLossType->setCurrentIndex(_settings.maximumLossType());
  _maximumLossValue->setValue(_settings.maximumLossValue());
  _maximumLossExit->setCurrentIndex(_settings.maximumLossExit());
  _profitTargetStop->setChecked((bool) _settings.profitTargetStop());
  _profitTargetType->setCurrentIndex(_settings.profitTargetType());
  _profitTargetValue->setValue(_settings.profitTargetValue());
  _profitTargetExit->setCurrentIndex(_settings.profitTargetExit());
  _trailingStop->setChecked((bool) _settings.trailingStop());
  _trailingType->setCurrentIndex(_settings.trailingType());
  _trailingValue->setValue(_settings.trailingValue());
  _trailingExit->setCurrentIndex(_settings.trailingExit());
  _barsStop->setChecked((bool) _settings.barsStop());
  _barsStopValue->setValue(_settings.barsStopValue());
  _barsStopExit->setCurrentIndex(_settings.barsStopExit());
  
  _enterLong->setIndicator(_settings.eLIndicator());
  _exitLong->setIndicator(_settings.xLIndicator());
  _enterShort->setIndicator(_settings.eSIndicator());
  _exitShort->setIndicator(_settings.xSIndicator());

  _enterLong->setList(_settings.enterLong());
  _exitLong->setList(_settings.exitLong());
  _enterShort->setList(_settings.enterShort());
  _exitShort->setList(_settings.exitShort());
  
  _enterLong->setSettings(_settings.eLSettings());
  _exitLong->setSettings(_settings.xLSettings());
  _enterShort->setSettings(_settings.eSSettings());
  _exitShort->setSettings(_settings.xSSettings());

  _symbols->setSymbolList(_settings.symbols());

  _saveFlag = 0;
  _saveButton->setEnabled(FALSE);
}

void TesterSettingsDialog::saveSettings ()
{
  TesterConfig config;
  config.transaction();

  // save app size and position
  QString k = "size" + _settings.name();
  QSize sz = size();
  config.setData(k, sz);

  k = "position" + _settings.name();
  QPoint pt = pos();
  config.setData(k, pt);

  config.commit();
  
  if (! _saveFlag)
    return;

  _settings.setBarsStopExit(_barsStopExit->currentIndex());
  _settings.setBarsStopValue(_barsStopValue->value());
  _settings.setTrailingExit(_trailingExit->currentIndex());
  _settings.setTrailingValue(_trailingValue->value());
  _settings.setTrailingType(_trailingType->currentIndex());
  _settings.setProfitTargetExit(_profitTargetExit->currentIndex());
  _settings.setProfitTargetValue(_profitTargetValue->value());
  _settings.setProfitTargetType(_profitTargetType->currentIndex());
  _settings.setMaximumLossExit(_maximumLossExit->currentIndex());
  _settings.setMaximumLossValue(_maximumLossValue->value());
  _settings.setMaximumLossType(_maximumLossType->currentIndex());
  _settings.setShortSellPrice(_shortSellPrice->currentIndex());
  _settings.setShortBuyPrice(_shortBuyPrice->currentIndex());
  _settings.setLongSellPrice(_longSellPrice->currentIndex());
  _settings.setLongBuyPrice(_longBuyPrice->currentIndex());
  _settings.setFuturesMode(_futuresMode->isChecked());
  _settings.setCommissionValue(_commissionValue->value());
  _settings.setCommission(_commission->currentIndex());
  _settings.setPositionSize(_positionSize->value());
  _settings.setDateRange(_dateRange->currentIndex());
  _settings.setPeriod(_period->currentIndex());
  _settings.setEquity(_equity->value());
  _symbols->symbolList(_settings.symbols());
  _settings.setLong((int) _long->isChecked());
  _settings.setShort((int) _short->isChecked());
  _settings.setMaximumLossStop((int) _maximumLossStop->isChecked());
  _settings.setProfitTargetStop((int) _profitTargetStop->isChecked());
  _settings.setTrailingStop((int) _trailingStop->isChecked());
  _settings.setBarsStop((int) _barsStop->isChecked());

  _settings.setELIndicator(_enterLong->indicator());
  _enterLong->list(_settings.enterLong());
  _settings.setELSettings(_enterLong->settings());

  _settings.setXLIndicator(_exitLong->indicator());
  _exitLong->list(_settings.exitLong());
  _settings.setXLSettings(_exitLong->settings());

  _settings.setESIndicator(_enterShort->indicator());
  _enterShort->list(_settings.enterShort());
  _settings.setESSettings(_enterShort->settings());

  _settings.setXSIndicator(_exitLong->indicator());
  _exitShort->list(_settings.exitShort());
  _settings.setXSSettings(_exitShort->settings());

  TesterDataBase db;
  db.transaction();
  db.setRule(_settings);
  db.commit();

  _saveFlag = 0;
  _saveButton->setEnabled(FALSE);
}

void TesterSettingsDialog::done ()
{
  if (_saveFlag)
    saveSettings();
  
  accept();
}

void TesterSettingsDialog::confirmYes ()
{
  done();
}

void TesterSettingsDialog::cancel ()
{
  if (_saveFlag)
  {
    setMessage(tr("Settings modified. Save changes?"));
    setConfirm();
    _saveButton->setEnabled(FALSE);
    _closeButton->setEnabled(FALSE);
  }
  else
    reject();
}

void TesterSettingsDialog::trailingTypeChanged ()
{
  if (_trailingType->currentIndex() == 0)
    _trailingValue->setRange(0, 100);
  else
    _trailingValue->setRange(0, 99999999);

  ruleChanged();
}

void TesterSettingsDialog::profitTargetTypeChanged ()
{
  if (_profitTargetType->currentIndex() == 0)
    _profitTargetValue->setRange(0, 100);
  else
    _profitTargetValue->setRange(0, 99999999);

  ruleChanged();
}

void TesterSettingsDialog::maximumLossTypeChanged ()
{
  if (_maximumLossType->currentIndex() == 0)
    _maximumLossValue->setRange(0, 100);
  else
    _maximumLossValue->setRange(0, 99999999);

  ruleChanged();
}

void TesterSettingsDialog::ruleChanged ()
{
  _saveFlag = TRUE;
  _saveButton->setEnabled(_saveFlag);
}
