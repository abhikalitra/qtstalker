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

#include <QLayout>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>

#include "TesterTestPage.h"
#include "BarData.h"




TesterTestPage::TesterTestPage (QWidget *p) : QWidget (p)
{
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(5);
  hbox->setSpacing(10);
  setLayout(hbox);

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(10);
  hbox->addLayout(vbox);

  // left side grid

  QGridLayout *grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  // trades area  

  tradeLong = new QCheckBox(tr("Long"));
  grid->addWidget(tradeLong, 0, 0);

  tradeShort = new QCheckBox(tr("Short"));
  grid->addWidget(tradeShort, 1, 0);

  QFrame *line = new QFrame;
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(line);

  // account area

  grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  QLabel *label = new QLabel(tr("Account Balance"));
  grid->addWidget(label, 0, 0);

  account = new QDoubleSpinBox;
  account->setRange(0.0, 999999999.0);
  account->setValue(10000.0);
  account->setDecimals(2);
  grid->addWidget(account, 0, 1);
  
  label = new QLabel(tr("Volume %"));
  grid->addWidget(label, 1, 0);

  volumePercent = new QDoubleSpinBox;
  volumePercent->setRange(0, 100);
  volumePercent->setDecimals(2);
  grid->addWidget(volumePercent, 1, 1);

  line = new QFrame;
  line->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  vbox->addWidget(line);

  // commission area

  grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  vbox->addLayout(grid);

  commissionType = new QCheckBox(tr("Use Commission %"));
  grid->addWidget(commissionType, 0, 0);

  label = new QLabel(tr("Entry"));
  grid->addWidget(label, 1, 0);

  entryCom = new QDoubleSpinBox;
  entryCom->setRange(0.0, 99999999.0);
  entryCom->setDecimals(2);
  entryCom->setValue(10.0);
  grid->addWidget(entryCom, 1, 1);

  label = new QLabel(tr("Exit"));
  grid->addWidget(label, 2, 0);

  exitCom = new QDoubleSpinBox;
  exitCom->setRange(0.0, 99999999.0);
  exitCom->setDecimals(2);
  exitCom->setValue(10.0);
  grid->addWidget(exitCom, 2, 1);

  vbox->addStretch(1);

  // vline sperarator
  
  line = new QFrame;
  line->setFrameStyle(QFrame::VLine | QFrame::Sunken);
  hbox->addWidget(line);

  // right side grid

  grid = new QGridLayout;
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  hbox->addLayout(grid);

  // test area

  label = new QLabel(tr("Symbol"));
  grid->addWidget(label, 0, 0);

  QString s, s2;
  QStringList l;
  symbolButton = new SymbolButton(this, l);
  grid->addWidget(symbolButton, 0, 1);
  
  label = new QLabel(tr("Bar Length"));
  grid->addWidget(label, 1, 0);
  
  BarData bd(s);
  barLength = new QComboBox;
  bd.getBarLengthList(barLengthList);
  barLength->addItems(barLengthList);
  grid->addWidget(barLength, 1, 1);

  label = new QLabel(tr("Bars"));
  grid->addWidget(label, 2, 0);
  
  bars = new QSpinBox;
  bars->setRange(1, 99999999);
  bars->setValue(275);
  grid->addWidget(bars, 2, 1);

  label = new QLabel(tr("Entry/Exit Price"));
  grid->addWidget(label, 3, 0);
  
  priceField = new QComboBox;
  fieldList.append(tr("Open"));
  fieldList.append(tr("Close"));
  fieldList.append(tr("Mid Point"));
  priceField->addItems(fieldList);
  grid->addWidget(priceField, 3, 1);

  grid->setRowStretch(grid->rowCount(), 1);
}

void TesterTestPage::setParms (TesterRule &rule)
{
  QString s;
  rule.getData(TesterRule::PriceField, s);
  priceField->setCurrentIndex(priceField->findText(s));

  rule.getData(TesterRule::BarLength, s);
  barLength->setCurrentIndex(barLength->findText(s));

  QStringList l;
  rule.getSymbols(l);
  symbolButton->setSymbols(l);

  tradeLong->setChecked(rule.getInt(TesterRule::TradeLong));
  tradeShort->setChecked(rule.getInt(TesterRule::TradeShort));
  volumePercent->setValue(rule.getDouble(TesterRule::VolumePercent));
  entryCom->setValue(rule.getDouble(TesterRule::EntryCom));
  exitCom->setValue(rule.getDouble(TesterRule::EntryCom));
  bars->setValue(rule.getInt(TesterRule::Bars));
  account->setValue(rule.getDouble(TesterRule::Account));
  commissionType->setChecked(rule.getInt(TesterRule::CommissionType));
}

void TesterTestPage::getParms (TesterRule &rule)
{
  QString s = priceField->currentText();
  rule.setData(TesterRule::PriceField, s);

  s = barLength->currentText();
  rule.setData(TesterRule::BarLength, s);

  QStringList l;
  symbolButton->getSymbols(l);
  rule.setSymbols(l);

  rule.setInt(TesterRule::TradeLong, tradeLong->isChecked());
  rule.setInt(TesterRule::TradeShort, tradeShort->isChecked());
  rule.setDouble(TesterRule::VolumePercent, volumePercent->value());
  rule.setDouble(TesterRule::EntryCom, entryCom->value());
  rule.setDouble(TesterRule::EntryCom, exitCom->value());
  rule.setInt(TesterRule::Bars, bars->value());
  rule.setDouble(TesterRule::Account, account->value());
  rule.setInt(TesterRule::CommissionType, commissionType->isChecked());
}

