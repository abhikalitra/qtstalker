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

  customLongStopEdit = new FormulaEdit;  
  tvbox->addWidget(customLongStopEdit);

  customShortBox = new QGroupBox;
  customShortBox->setCheckable(TRUE);
  customShortBox->setTitle(tr("Custom Short"));
  customShortBox->setChecked(FALSE);
  hbox->addWidget(customShortBox);
    
  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(5);
  customShortBox->setLayout(tvbox);

  customShortStopEdit = new FormulaEdit;  
  tvbox->addWidget(customShortStopEdit);

  vbox->addStretch(1);
}

void TesterStopPage::getStops (TesterRule &rule)
{
  Indicator i;
  customLongStopEdit->getIndicator(i);
  rule.setCustomLongStop(i);
  rule.setCustomLongCheck(customLongBox->isChecked());

  customShortStopEdit->getIndicator(i);
  rule.setCustomShortStop(i);
  rule.setCustomShortCheck(customShortBox->isChecked());

  rule.setMaxLossCheck(maxLossBox->isChecked());
  rule.setMaxLossLong(maximumLossLong->isChecked());
  rule.setMaxLossShort(maximumLossShort->isChecked());
  rule.setMaxLoss(maxLoss->value());

  rule.setProfitCheck(profitBox->isChecked());
  rule.setProfitLong(profitLong->isChecked());
  rule.setProfitShort(profitShort->isChecked());
  rule.setProfit(profit->value());

  rule.setTrailingCheck(trailingBox->isChecked());
  rule.setTrailingLong(trailingLong->isChecked());
  rule.setTrailingShort(trailingShort->isChecked());
  rule.setTrailing(trailing->value());
}

void TesterStopPage::setStops (TesterRule &rule)
{
  Indicator i;
  rule.getCustomLongStop(i);
  customLongStopEdit->setIndicator(i);
  customLongBox->setChecked(rule.getCustomLongCheck());
  
  rule.getCustomShortStop(i);
  customShortStopEdit->setIndicator(i);
  customShortBox->setChecked(rule.getCustomShortCheck());

  maxLossBox->setChecked(rule.getMaxLossCheck());
  maximumLossLong->setChecked(rule.getMaxLossLong());
  maximumLossShort->setChecked(rule.getMaxLossShort());
  maxLoss->setValue(rule.getMaxLoss());

  profitBox->setChecked(rule.getProfitCheck());
  profitLong->setChecked(rule.getProfitLong());
  profitShort->setChecked(rule.getProfitShort());
  profit->setValue(rule.getProfit());

  trailingBox->setChecked(rule.getTrailingCheck());
  trailingLong->setChecked(rule.getTrailingLong());
  trailingShort->setChecked(rule.getTrailingShort());
  trailing->setValue(rule.getTrailing());
}

