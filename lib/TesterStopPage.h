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

#ifndef TESTERSTOPPAGE_HPP
#define TESTERSTOPPAGE_HPP

#include <QWidget>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QGroupBox>

#include "FormulaEdit.h"
#include "TesterRule.h"



class TesterStopPage : public QWidget
{
  Q_OBJECT
  
  public:
    TesterStopPage (QWidget *);
    void getStops (TesterRule &);
    void setStops (TesterRule &);
    void createStopsPage ();

  protected:
    QGroupBox *maxLossBox;
    QCheckBox *maximumLossLong;
    QCheckBox *maximumLossShort;
    QDoubleSpinBox *maxLoss;

    QGroupBox *profitBox;
    QCheckBox *profitLong;
    QCheckBox *profitShort;
    QDoubleSpinBox *profit;

    QGroupBox *trailingBox;
    QCheckBox *trailingLong;
    QCheckBox *trailingShort;
    QDoubleSpinBox *trailing;

    QGroupBox *customLongBox;
    QGroupBox *customShortBox;
    FormulaEdit *customShortStopEdit;
    FormulaEdit *customLongStopEdit;
};

#endif
