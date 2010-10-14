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

#ifndef TESTER_SETTINGS_DIALOG_HPP
#define TESTER_SETTINGS_DIALOG_HPP

#include <QDialog>
#include <QComboBox>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTabWidget>
#include <QGroupBox>

#include "SymbolButton.h"
#include "Indicator.h"
#include "TesterSettings.h"
#include "IndicatorPlotList.h"

class TesterSettingsDialog : public QDialog
{
  Q_OBJECT
  
  public:
    TesterSettingsDialog (QString);
    void createGeneralPage ();
    void createTradesPage ();
    void createStopsPage ();
    void createIndicatorPage ();
    void loadSettings ();
    
  public slots:
    void saveSettings ();
    void ruleChanged ();
    void trailingTypeChanged ();
    void profitTargetTypeChanged ();
    void maximumLossTypeChanged ();
    void closeDialog ();
          
  private:
    QTabWidget *_tabs;
    int _saveFlag;
    TesterSettings _settings;
    QPushButton *_saveButton;
    QPushButton *_closeButton;
    SymbolButton *_symbols;
    QDoubleSpinBox *_equity;
    QComboBox *_period;
    QComboBox *_dateRange;
    QDoubleSpinBox *_positionSize;
    QCheckBox *_futuresMode;
    QComboBox *_commission;
    QDoubleSpinBox *_commissionValue;
    QGroupBox *_long;
    QComboBox *_longBuyPrice;
    QComboBox *_longSellPrice;
    QGroupBox *_short;
    QComboBox *_shortBuyPrice;
    QComboBox *_shortSellPrice;
    QGroupBox *_maximumLossStop;
    QComboBox *_maximumLossType;
    QDoubleSpinBox *_maximumLossValue;
    QComboBox *_maximumLossExit;
    QGroupBox *_profitTargetStop;
    QComboBox *_profitTargetType;
    QDoubleSpinBox *_profitTargetValue;
    QComboBox *_profitTargetExit;
    QGroupBox *_trailingStop;
    QComboBox *_trailingType;
    QDoubleSpinBox *_trailingValue;
    QComboBox *_trailingExit;
    QGroupBox *_barsStop;
    QSpinBox *_barsStopValue;
    QComboBox *_barsStopExit;
    QComboBox *_indicator;
    IndicatorPlotList *_enterLong;
    IndicatorPlotList *_exitLong;
    IndicatorPlotList *_enterShort;
    IndicatorPlotList *_exitShort;
};

#endif

