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
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "TesterStopPage.h"




TesterStopPage::TesterStopPage (QWidget *p) : QWidget (p)
{
  createStopsPage();
}

void TesterStopPage::createStopsPage ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(10);
  vbox->setSpacing(5);
  setLayout(vbox);

  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);

  maxLossBox = new QGroupBox;
  maxLossBox->setCheckable(TRUE);
  maxLossBox->setTitle(tr("Maximum Loss"));
  maxLossBox->setChecked(FALSE);
  hbox->addWidget(maxLossBox);
    
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  maxLossBox->setLayout(tvbox);
  
  maximumLossLong = new QCheckBox(tr("Long"));
  tvbox->addWidget(maximumLossLong);

  maximumLossShort = new QCheckBox(tr("Short"));
  tvbox->addWidget(maximumLossShort);

  maxLoss = new QDoubleSpinBox;
//  maxLoss->setTitle(tr("Loss %"));
  tvbox->addWidget(maxLoss);

  profitBox = new QGroupBox;
  profitBox->setCheckable(TRUE);
  profitBox->setTitle(tr("Profit"));
  profitBox->setChecked(FALSE);
  hbox->addWidget(profitBox);
    
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  profitBox->setLayout(tvbox);
  
  profitLong = new QCheckBox(tr("Long"));
  tvbox->addWidget(profitLong);

  profitShort = new QCheckBox(tr("Short"));
  tvbox->addWidget(profitShort);

  profit = new QDoubleSpinBox;
//  profit->setTitle(tr("Loss %"));
  tvbox->addWidget(profit);

  trailingBox = new QGroupBox;
  trailingBox->setCheckable(TRUE);
  trailingBox->setTitle(tr("Trailing"));
  trailingBox->setChecked(FALSE);
  hbox->addWidget(trailingBox);
    
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  trailingBox->setLayout(tvbox);
  
  trailingLong = new QCheckBox(tr("Long"));
  tvbox->addWidget(trailingLong);

  trailingShort = new QCheckBox(tr("Short"));
  tvbox->addWidget(trailingShort);

  trailing = new QDoubleSpinBox;
//  trailing->setTitle(tr("Trailing %"));
  tvbox->addWidget(trailing);

  hbox = new QHBoxLayout;
  hbox->setMargin(0);
  hbox->setSpacing(5);
  vbox->addLayout(hbox);
    
  customLongBox = new QGroupBox;
  customLongBox->setCheckable(TRUE);
  customLongBox->setTitle(tr("Custom Long"));
  customLongBox->setChecked(FALSE);
  hbox->addWidget(customLongBox);
    
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  customLongBox->setLayout(tvbox);

//  customLongStopEdit = new FormulaEdit;  
//  tvbox->addWidget(customLongStopEdit);

  customShortBox = new QGroupBox;
  customShortBox->setCheckable(TRUE);
  customShortBox->setTitle(tr("Custom Short"));
  customShortBox->setChecked(FALSE);
  hbox->addWidget(customShortBox);
    
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  customShortBox->setLayout(tvbox);

//  customShortStopEdit = new FormulaEdit;  
//  tvbox->addWidget(customShortStopEdit);

  vbox->addStretch(1);
}

void TesterStopPage::getStops (TesterRule &rule)
{
  Indicator i;
//  customLongStopEdit->getIndicator(i);
  rule.setCustomLongStop(i);
  rule.setInt(TesterRule::CustomLongCheck, customLongBox->isChecked());

//  customShortStopEdit->getIndicator(i);
  rule.setCustomShortStop(i);
  rule.setInt(TesterRule::CustomShortCheck, customShortBox->isChecked());

  rule.setInt(TesterRule::MaxLossCheck, maxLossBox->isChecked());
  rule.setInt(TesterRule::MaxLossLong, maximumLossLong->isChecked());
  rule.setInt(TesterRule::MaxLossShort, maximumLossShort->isChecked());
  rule.setDouble(TesterRule::MaxLoss, maxLoss->value());

  rule.setInt(TesterRule::ProfitCheck, profitBox->isChecked());
  rule.setInt(TesterRule::ProfitLong, profitLong->isChecked());
  rule.setInt(TesterRule::ProfitShort, profitShort->isChecked());
  rule.setDouble(TesterRule::Profit, profit->value());

  rule.setInt(TesterRule::TrailingCheck, trailingBox->isChecked());
  rule.setInt(TesterRule::TrailingLong, trailingLong->isChecked());
  rule.setInt(TesterRule::TrailingShort, trailingShort->isChecked());
  rule.setDouble(TesterRule::Trailing, trailing->value());
}

void TesterStopPage::setStops (TesterRule &rule)
{
  Indicator i;
  rule.getCustomLongStop(i);
//  customLongStopEdit->setIndicator(i);
  customLongBox->setChecked(rule.getInt(TesterRule::CustomLongCheck));
  
  rule.getCustomShortStop(i);
//  customShortStopEdit->setIndicator(i);
  customShortBox->setChecked(rule.getInt(TesterRule::CustomShortCheck));

  maxLossBox->setChecked(rule.getInt(TesterRule::MaxLossCheck));
  maximumLossLong->setChecked(rule.getInt(TesterRule::MaxLossLong));
  maximumLossShort->setChecked(rule.getInt(TesterRule::MaxLossShort));
  maxLoss->setValue(rule.getDouble(TesterRule::MaxLoss));

  profitBox->setChecked(rule.getInt(TesterRule::ProfitCheck));
  profitLong->setChecked(rule.getInt(TesterRule::ProfitLong));
  profitShort->setChecked(rule.getInt(TesterRule::ProfitShort));
  profit->setValue(rule.getDouble(TesterRule::Profit));

  trailingBox->setChecked(rule.getInt(TesterRule::TrailingCheck));
  trailingLong->setChecked(rule.getInt(TesterRule::TrailingLong));
  trailingShort->setChecked(rule.getInt(TesterRule::TrailingShort));
  trailing->setValue(rule.getDouble(TesterRule::Trailing));
}

