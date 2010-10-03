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
  _thread = 0;
  _runningFlag = 0;
  _settings.setData(TesterDataBase::_Name, name);

  QString s = "Qtstalker: " + tr("Tester") + " - " + name;
  setWindowTitle(s);

  QPushButton *b = _buttonBox->button(QDialogButtonBox::Cancel);
  _buttonBox->removeButton(b);
  
  b = _buttonBox->button(QDialogButtonBox::Ok);
  _buttonBox->removeButton(b);

  _saveButton = _buttonBox->addButton(QDialogButtonBox::Save);
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

  longChanged (0);
  shortChanged (0);
  maximumLossStopChanged (0);
  profitTargetStopChanged (0);
  trailingStopChanged (0);
  barsStopChanged (0);
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
  _equity->setValue(10000);
  connect(_equity, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_equity, row++, col--);
  
  // positions
  label = new QLabel(tr("Positions"));
  grid->addWidget(label, row, col++);

  QStringList l;
  l << tr("Long") << tr("Short") << tr("Long/Short");
  
  _position = new QComboBox;
  _position->addItems(l);
  connect(_position, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_position, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  l.clear();
  BarData bd;
  bd.getBarLengthList(l);

  _period = new QComboBox;
  _period->addItems(l);
  _period->setCurrentIndex(6);
  connect(_period, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
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
  connect(_dateRange, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_dateRange, row++, col--);

  

  // position size
  label = new QLabel(tr("Position Size"));
  grid->addWidget(label, row, col++);

  _positionSize = new QDoubleSpinBox;
  _positionSize->setRange(0.01, 1.0);
  _positionSize->setValue(0.1);
  connect(_positionSize, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_positionSize, row++, col--);

  // commission
  label = new QLabel(tr("Commission"));
  grid->addWidget(label, row, col++);

  l.clear();
  l << tr("Percent") << tr("$ Per Trade") << tr("$ Per Share");
  
  _commission = new QComboBox;
  _commission->addItems(l);
  _commission->setCurrentIndex(1);
  connect(_commission, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_commission, row, col++);

  _commissionValue = new QDoubleSpinBox;
  _commissionValue->setRange(0.01, 9999);
  _commissionValue->setValue(10);
  connect(_commissionValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_commissionValue, row++, col);
  col -= 2;

  // symbols
  label = new QLabel(tr("Symbols"));
  grid->addWidget(label, row, col++);

  _symbols = new SymbolButton(w);
  grid->addWidget(_symbols, row++, col--);
  
  // futures mode
  label = new QLabel(tr("Futures Mode"));
  grid->addWidget(label, row, col++);

  _futuresMode = new QCheckBox;
  connect(_futuresMode, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
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
  connect(_long, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_long, SIGNAL(stateChanged(int)), this, SLOT(longChanged(int)));
  grid->addWidget(_long, row, col++);
  
  QStringList l;
  BarData bd;
  bd.getInputFields(l);

  _longBuyPrice = new QComboBox;
  _longBuyPrice->addItems(l);
  connect(_longBuyPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_longBuyPrice, row, col++);

  _longSellPrice = new QComboBox;
  _longSellPrice->addItems(l);
  _longSellPrice->setCurrentIndex(3);
  connect(_longSellPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_longSellPrice, row++, col);
  col -= 2;

  // short trades
  _short = new QCheckBox(tr("Short"));
  connect(_short, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_short, SIGNAL(stateChanged(int)), this, SLOT(shortChanged(int)));
  grid->addWidget(_short, row, col++);

  _shortBuyPrice = new QComboBox;
  _shortBuyPrice->addItems(l);
  connect(_shortBuyPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_shortBuyPrice, row, col++);

  _shortSellPrice = new QComboBox;
  _shortSellPrice->addItems(l);
  _shortSellPrice->setCurrentIndex(3);
  connect(_shortSellPrice, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_shortSellPrice, row++, col);
  col -= 2;

  vbox->addStretch();

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
  connect(_maximumLossStop, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_maximumLossStop, SIGNAL(stateChanged(int)), this, SLOT(maximumLossStopChanged(int)));
  grid->addWidget(_maximumLossStop, row, col++);

  QStringList l;
  l << tr("Percent") << tr("Point");

  _maximumLossType = new QComboBox;
  _maximumLossType->addItems(l);
  connect(_maximumLossType, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_maximumLossType, row, col++);

  _maximumLossValue = new QDoubleSpinBox;
  _maximumLossValue->setRange(0.01, 99999999);
  _maximumLossValue->setValue(0.1);
  connect(_maximumLossValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_maximumLossValue, row, col++);

  QStringList l2;
  BarData bd;
  bd.getInputFields(l2);

  _maximumLossExit = new QComboBox;
  _maximumLossExit->addItems(l2);
  _maximumLossExit->setCurrentIndex(3);
  connect(_maximumLossExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_maximumLossExit, row++, col);
  col -= 3;

  // profit target
  _profitTargetStop = new QCheckBox(tr("Profit Target"));
  connect(_profitTargetStop, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_profitTargetStop, SIGNAL(stateChanged(int)), this, SLOT(profitTargetStopChanged(int)));
  grid->addWidget(_profitTargetStop, row, col++);

  _profitTargetType = new QComboBox;
  _profitTargetType->addItems(l);
  connect(_profitTargetType, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_profitTargetType, row, col++);

  _profitTargetValue = new QDoubleSpinBox;
  _profitTargetValue->setRange(0.01, 99999999);
  _profitTargetValue->setValue(0.1);
  connect(_profitTargetValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_profitTargetValue, row, col++);

  _profitTargetExit = new QComboBox;
  _profitTargetExit->addItems(l2);
  _profitTargetExit->setCurrentIndex(3);
  connect(_profitTargetExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_profitTargetExit, row++, col);
  col -= 3;

  // trailing stop
  _trailingStop = new QCheckBox(tr("Trailing"));
  connect(_trailingStop, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_trailingStop, SIGNAL(stateChanged(int)), this, SLOT(trailingStopChanged(int)));
  grid->addWidget(_trailingStop, row, col++);

  _trailingType = new QComboBox;
  _trailingType->addItems(l);
  connect(_trailingType, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_trailingType, row, col++);

  _trailingValue = new QDoubleSpinBox;
  _trailingValue->setRange(0.01, 99999999);
  _trailingValue->setValue(0.1);
  connect(_trailingValue, SIGNAL(valueChanged(double)), this, SLOT(ruleChanged()));
  grid->addWidget(_trailingValue, row, col++);

  _trailingExit = new QComboBox;
  _trailingExit->addItems(l2);
  _trailingExit->setCurrentIndex(3);
  connect(_trailingExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_trailingExit, row++, col);
  col -= 3;

  // bars stop
  _barsStop = new QCheckBox(tr("Bars"));
  connect(_barsStop, SIGNAL(stateChanged(int)), this, SLOT(ruleChanged()));
  connect(_barsStop, SIGNAL(stateChanged(int)), this, SLOT(barsStopChanged(int)));
  grid->addWidget(_barsStop, row, col++);

  _barsStopValue = new QSpinBox;
  _barsStopValue->setRange(1, 99999999);
  _barsStopValue->setValue(50);
  connect(_barsStopValue, SIGNAL(valueChanged(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_barsStopValue, row, col++);

  _barsStopExit = new QComboBox;
  _barsStopExit->addItems(l2);
  _barsStopExit->setCurrentIndex(3);
  connect(_barsStopExit, SIGNAL(activated(int)), this, SLOT(ruleChanged()));
  grid->addWidget(_barsStopExit, row++, col);
  col -= 2;

  vbox->addStretch();

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
  grid->setSpacing(2);
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
  _indicator->setCurrentIndex(0);
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

  _report = new QTextEdit;
  _report->setReadOnly(TRUE);
  vbox->addWidget(_report);

//  vbox->addStretch();

  _tabs->addTab(w, tr("Report"));
}

void TesterSettingsDialog::loadSettings ()
{
  TesterDataBase db;
  if (db.getRule(_settings))
    return;
}

void TesterSettingsDialog::saveSettings ()
{
  if (! _saveFlag)
    return;

  TesterDataBase db;
  db.transaction();
  db.setRule(_settings);
  db.commit();
}

void TesterSettingsDialog::ruleChanged ()
{
  _saveFlag = TRUE;
  _saveButton->setEnabled(_saveFlag);
}

void TesterSettingsDialog::longChanged (int state)
{
  _longBuyPrice->setEnabled(state);
  _longSellPrice->setEnabled(state);
}

void TesterSettingsDialog::shortChanged (int state)
{
  _shortBuyPrice->setEnabled(state);
  _shortSellPrice->setEnabled(state);
}

void TesterSettingsDialog::maximumLossStopChanged (int state)
{
  _maximumLossType->setEnabled(state);
  _maximumLossValue->setEnabled(state);
  _maximumLossExit->setEnabled(state);
}

void TesterSettingsDialog::profitTargetStopChanged (int state)
{
  _profitTargetType->setEnabled(state);
  _profitTargetValue->setEnabled(state);
  _profitTargetExit->setEnabled(state);
}

void TesterSettingsDialog::trailingStopChanged (int state)
{
  _trailingType->setEnabled(state);
  _trailingValue->setEnabled(state);
  _trailingExit->setEnabled(state);
}

void TesterSettingsDialog::barsStopChanged (int state)
{
  _barsStopValue->setEnabled(state);
  _barsStopExit->setEnabled(state);
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

  Indicator i;
  if (! _indicatorSettings.count())
  {
    plugin->defaults(i);
    _indicatorSettings = i.settings();
  }
  i.setSettings(_indicatorSettings);
  
  QStringList plotNames;
  plugin->plotNames(i, plotNames);

  setIndicatorPlots(plotNames, _enterLong);
  setIndicatorPlots(plotNames, _exitLong);
  setIndicatorPlots(plotNames, _enterShort);
  setIndicatorPlots(plotNames, _exitShort);
}

void TesterSettingsDialog::setIndicatorPlots (QStringList plotNames, QTreeWidget *tree)
{
  tree->clear();

  int loop = 0;
  for (; loop < plotNames.count(); loop++)
  {
    QTreeWidgetItem *item = new QTreeWidgetItem(tree);

    item->setCheckState(0, (Qt::CheckState) 0);

    item->setText(1, plotNames.at(loop));

    QComboBox *cb = new QComboBox;
    Operator op;
    cb->addItems(op.list());
    cb->setCurrentIndex(0);
    tree->setItemWidget(item, 2, cb);

    QComboBox *vc = new QComboBox;
    vc->setEditable(TRUE);
    vc->addItems(plotNames);
    vc->clearEditText();
    tree->setItemWidget(item, 3, vc);
  }

  for (loop = 0; loop < tree->topLevelItemCount(); loop++)
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
  if (! _indicatorSettings.count())
  {
    plug->defaults(i);
    _indicatorSettings = i.settings();
  }
  i.setSettings(_indicatorSettings);

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
  _indicatorSettings = i.settings();
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

void TesterSettingsDialog::done (QString report)
{
  _runningFlag = 0;
  this->setEnabled(FALSE);

  _closeButton->setText(tr("Close"));

  _report->clear();
  _report->append(report);
  ruleChanged();
}

void TesterSettingsDialog::run ()
{
  if (_thread)
    delete _thread;

  Indicator i;
  i.setSettings(_indicatorSettings);
  
  _thread = new TesterThread(this, i);
  connect(_thread, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  _thread->start();

  _runningFlag = 1;
  
  this->setEnabled(FALSE);
  
  _closeButton->setText(tr("Stop"));
  _closeButton->setEnabled(TRUE);
}

