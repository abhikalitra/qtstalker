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
#include <QGroupBox>
#include <QVBoxLayout>
#include <QGridLayout>

#include "TesterRulePage.h"




TesterRulePage::TesterRulePage (QWidget *p) : QWidget (p)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(5);
  vbox->setSpacing(5);
  setLayout(vbox);
  
  QGridLayout *grid = new QGridLayout;
  vbox->addLayout(grid);
  
  QGroupBox *gbox = new QGroupBox(tr("Enter Long"));
  grid->addWidget(gbox, 0, 0);

  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

//  enterLongEdit = new FormulaEdit;  
//  tvbox->addWidget(enterLongEdit);
  
  gbox = new QGroupBox(tr("Exit Long"));
  grid->addWidget(gbox, 0, 1);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

//  exitLongEdit = new FormulaEdit;  
//  tvbox->addWidget(exitLongEdit);

  gbox = new QGroupBox(tr("Enter Short"));
  grid->addWidget(gbox, 1, 0);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

//  enterShortEdit = new FormulaEdit;  
//  tvbox->addWidget(enterShortEdit);
  
  gbox = new QGroupBox(tr("Exit Short"));
  grid->addWidget(gbox, 1, 1);

  tvbox = new QVBoxLayout;
  tvbox->setMargin(5);
  tvbox->setSpacing(0);
  gbox->setLayout(tvbox);

//  exitShortEdit = new FormulaEdit;  
//  tvbox->addWidget(exitShortEdit);
}

void TesterRulePage::getRules (TesterRule &rule)
{
  Indicator i;
//  enterLongEdit->getIndicator(i);
  rule.setEnterLong(i);

//  exitLongEdit->getIndicator(i);
  rule.setExitLong(i);

//  enterShortEdit->getIndicator(i);
  rule.setEnterShort(i);

//  exitShortEdit->getIndicator(i);
  rule.setExitShort(i);
}

void TesterRulePage::setRules (TesterRule &rule)
{
  Indicator i;
  rule.getEnterLong(i);
//  enterLongEdit->setIndicator(i);

  rule.getExitLong(i);
//  exitLongEdit->setIndicator(i);

  rule.getEnterShort(i);
//  enterShortEdit->setIndicator(i);

  rule.getExitShort(i);
//  exitShortEdit->setIndicator(i);
}

