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
#include <QValidator>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "TesterTestPage.h"
#include "BarData.h"


TesterTestPage::TesterTestPage (QWidget *p) : QWidget (p)
{
  fieldList.append(tr("Open"));
  fieldList.append(tr("Close"));
  fieldList.append(tr("Mid Point"));

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->setMargin(5);
  hbox->setSpacing(10);

  // left side grid

  QGridLayout *grid = new QGridLayout(this);
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  hbox->addLayout(grid);

  // trades area  

  tradeLong = new QCheckBox(tr("Long"), this);
  grid->addWidget(tradeLong, 0, 0);

  tradeShort = new QCheckBox(tr("Short"), this);
  grid->addWidget(tradeShort, 1, 0);

  QLabel *label = new QLabel(tr("Trade Delay"), this);
  grid->addWidget(label, 2, 0);
    
  tradeDelay = new QSpinBox(this);
  tradeDelay->setRange(1, 999999);
  tradeDelay->setValue(1);
  grid->addWidget(tradeDelay, 2, 1);

//  grid->setRowSpacing(grid->numRows(), 25);

  // account area

  label = new QLabel(tr("Account Balance"), this);
  grid->addWidget(label, 4, 0);

  account = new QSpinBox(this);
  account->setRange(0, 999999);
  account->setValue(10000);
  grid->addWidget(account, 4, 1);
  
  label = new QLabel(tr("Futures Margin"), this);
  grid->addWidget(label, 5, 0);

  margin = new QSpinBox(this);
  margin->setRange(0, 999999);
  grid->addWidget(margin, 5, 1);
  
  label = new QLabel(tr("Volume %"), this);
  grid->addWidget(label, 6, 0);

  volumePercent = new QSpinBox(this);
  volumePercent->setRange(0, 100);
  grid->addWidget(volumePercent, 6, 1);

//  grid->setRowSpacing(grid->numRows(), 25);

  // commission area

  commissionType = new QCheckBox(tr("Use Commission %"), this);
  grid->addWidget(commissionType, 8, 0);

  label = new QLabel(tr("Entry"), this);
  grid->addWidget(label, 9, 0);

  entryCom = new QDoubleSpinBox(this);
  entryCom->setRange(0, 99999999.0);
  entryCom->setDecimals(4);
  entryCom->setValue(10);
  grid->addWidget(entryCom, 9, 1);

  label = new QLabel(tr("Exit"), this);
  grid->addWidget(label, 10, 0);

  exitCom = new QDoubleSpinBox(this);
  exitCom->setRange(0, 99999999.0);
  exitCom->setDecimals(4);
  exitCom->setValue(10);
  grid->addWidget(exitCom, 10, 1);

//  grid->setRowStretch(grid->numRows() + 1, 1);

  // vline sperarator
  
  QFrame *line = new QFrame(this);
  line->setFrameStyle(QFrame::VLine | QFrame::Plain);
  hbox->addWidget(line);

  // right side grid

  grid = new QGridLayout(this);
  grid->setColumnStretch(1, 1);
  grid->setSpacing(5);
  hbox->addLayout(grid);

  // test area

  label = new QLabel(tr("Symbol"), this);
  grid->addWidget(label, 0, 0);

  QString s, s2;
  symbolButton = new SymbolButton(this, s, s2);
  connect(symbolButton, SIGNAL(symbolChanged()), this, SLOT(symbolButtonPressed()));
  grid->addWidget(symbolButton, 0, 1);
  
  label = new QLabel(tr("Bar Length"), this);
  grid->addWidget(label, 1, 0);
  
  BarData bd(s);
  barLength = new QComboBox(this);
  bd.getBarLengthList(barLengthList);
  barLength->addItems(barLengthList);
  barLength->setCurrentIndex(6);
  grid->addWidget(barLength, 1, 1);

  label = new QLabel(tr("Bars"), this);
  grid->addWidget(label, 2, 0);
  
  bars = new QSpinBox(this);
  bars->setRange(1, 99999999);
  bars->setValue(275);
  grid->addWidget(bars, 2, 1);

  label = new QLabel(tr("Entry/Exit Price"), this);
  grid->addWidget(label, 3, 0);
  
  priceField = new QComboBox(this);
  priceField->addItems(fieldList);
  grid->addWidget(priceField, 3, 1);

//  grid->setRowStretch(grid->numRows() + 1, 1);
}

TesterTestPage::~TesterTestPage ()
{
}

void TesterTestPage::symbolButtonPressed ()
{
  QString symbol;
  symbolButton->getPath(symbol);
}

bool TesterTestPage::getTradeLong ()
{
  return tradeLong->isChecked();
}

void TesterTestPage::setTradeLong (bool d)
{
  tradeLong->setChecked(d);
}

bool TesterTestPage::getTradeShort ()
{
  return tradeShort->isChecked();
}

void TesterTestPage::setTradeShort (bool d)
{
  tradeShort->setChecked(d);
}

int TesterTestPage::getVolumePercent ()
{
  return volumePercent->value();
}

void TesterTestPage::setVolumePercent (int d)
{
  volumePercent->setValue(d);
}

double TesterTestPage::getEntryCom ()
{
  return entryCom->text().toDouble();
}

void TesterTestPage::setEntryCom (double d)
{
  entryCom->setValue(d);
}

double TesterTestPage::getExitCom ()
{
  return exitCom->text().toDouble();
}

void TesterTestPage::setExitCom (double d)
{
  exitCom->setValue(d);
}

int TesterTestPage::getTradeDelay ()
{
  return tradeDelay->value();
}

void TesterTestPage::setTradeDelay (int d)
{
  tradeDelay->setValue(d);
}

QString TesterTestPage::getPriceField ()
{
  return priceField->currentText();
}

void TesterTestPage::setPriceField (QString d)
{
  priceField->setCurrentIndex(priceField->findText(d));
}

int TesterTestPage::getBars ()
{
  return bars->value();
}

void TesterTestPage::setBars (int d)
{
  bars->setValue(d);
}

QString TesterTestPage::getSymbolPath ()
{
  QString s;
  symbolButton->getPath(s);  
  return s;
}

void TesterTestPage::setSymbol (QString d)
{
  symbolButton->setSymbol(d);
}

QString TesterTestPage::getSymbol ()
{
  QString s;
  symbolButton->getSymbol(s);
  return s;
}

int TesterTestPage::getAccount ()
{
  return account->value();
}

void TesterTestPage::setAccount (int d)
{
  account->setValue(d);
}

QString TesterTestPage::getBarLength ()
{
  return barLength->currentText();
}

void TesterTestPage::setBarLength (QString d)
{
  barLength->setCurrentIndex(barLength->findText(d));
}

int TesterTestPage::getBarLengthIndex ()
{
  return barLengthList.indexOf(barLength->currentText());
}

int TesterTestPage::getMargin ()
{
  return margin->value();
}

void TesterTestPage::setMargin (int d)
{
  margin->setValue(d);
}

bool TesterTestPage::getCommissionType ()
{
  return commissionType->isChecked();
}

void TesterTestPage::setCommissionType (bool d)
{
  commissionType->setChecked(d);
}


