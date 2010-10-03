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

#include <QComboBox>
#include <QStringList>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTreeWidget>

#include "Setting.h"
#include "Dialog.h"
#include "SymbolButton.h"
#include "Indicator.h"
#include "TesterThread.h"

class TesterSettingsDialog : public Dialog
{
  Q_OBJECT
  
  public:
    TesterSettingsDialog (QString);
    void createGeneralPage ();
    void createTradesPage ();
    void createStopsPage ();
    void createIndicatorPage ();
    void createReportPage ();
    void loadSettings ();
    
  public slots:
    void saveSettings ();
    void ruleChanged ();
    void done (QString);
    void longChanged (int);
    void shortChanged (int);
    void maximumLossStopChanged (int);
    void profitTargetStopChanged (int);
    void trailingStopChanged (int);
    void barsStopChanged (int);
    void indicatorChanged (int);
    void setIndicatorPlots (QStringList plotNames, QTreeWidget *tree);
    void editIndicator ();
    void editIndicator2 (Indicator);
    void closeDialog ();
    void run ();
          
  private:
    int _saveFlag;
    int _runningFlag;
    TesterThread *_thread;
    Setting _settings;
    Setting _indicatorSettings;
    QPushButton *_saveButton;
    QPushButton *_runButton;
    QPushButton *_closeButton;
    SymbolButton *_symbols;
    QDoubleSpinBox *_equity;
    QComboBox *_position;
    QComboBox *_period;
    QComboBox *_dateRange;
    QDoubleSpinBox *_positionSize;
    QCheckBox *_futuresMode;
    QComboBox *_commission;
    QDoubleSpinBox *_commissionValue;
    QCheckBox *_long;
    QComboBox *_longBuyPrice;
    QComboBox *_longSellPrice;
    QCheckBox *_short;
    QComboBox *_shortBuyPrice;
    QComboBox *_shortSellPrice;
    QCheckBox *_maximumLossStop;
    QComboBox *_maximumLossType;
    QDoubleSpinBox *_maximumLossValue;
    QComboBox *_maximumLossExit;
    QCheckBox *_profitTargetStop;
    QComboBox *_profitTargetType;
    QDoubleSpinBox *_profitTargetValue;
    QComboBox *_profitTargetExit;
    QCheckBox *_trailingStop;
    QComboBox *_trailingType;
    QDoubleSpinBox *_trailingValue;
    QComboBox *_trailingExit;
    QCheckBox *_barsStop;
    QSpinBox *_barsStopValue;
    QComboBox *_barsStopExit;
    QTextEdit *_report;
    QComboBox *_indicator;
    QTreeWidget *_enterLong;
    QTreeWidget *_exitLong;
    QTreeWidget *_enterShort;
    QTreeWidget *_exitShort;
};

#endif

