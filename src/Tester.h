/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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

#ifndef TESTER_HPP
#define TESTER_HPP

#include <qstring.h>
#include <qtabdialog.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qlabel.h>
#include <qtable.h>
#include <qcombobox.h>
#include <qslider.h>
#include <qsplitter.h>
#include "Config.h"
#include "Setting.h"
#include "BarData.h"
#include "SymbolButton.h"
#include "FormulaEdit.h"
#include "Plot.h"
#include "PlotLine.h"
#include "FuturesData.h"

class Tester : public QTabDialog
{
  Q_OBJECT
  
  signals:
    void signalIndex (int);
    void message (QString);

  public:
    Tester (QString);
    Tester ();
    ~Tester ();
    void createFormulaPage();
    void createStopPage();
    void createTestPage();
    void createReportPage();
    void createChartPage ();
    PlotLine * loadIndicator (FormulaEdit *);
    void createSummary ();
    int getVolume (int, double);
    double getPrice (int);
    void saveEditRule (int);
    void loadEditRule (int);
    void updateChart ();
//    void createEquityCurve ();
    bool loadCustomLongStop ();
    bool loadCustomShortStop ();
    void saveCustomStopRule ();
    void loadCustomStopRule ();
    QString newTest ();

    void loadSignals ();
    void enterLong ();
    void enterShort ();
    bool maximumLoss (bool flag, double enterPrice, double exitPrice);
    bool profit (bool flag, double enterPrice, double exitPrice);
    bool trailing (bool flag, double exitPrice);
    bool customStop (bool flag, int index);
    void updateTradeList ();
    void updateEquityCurve ();

  public slots:
    void test ();
    void maximumLossToggled (bool);
    void profitToggled (bool);
    void trailingToggled (bool);
    void symbolButtonPressed ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void slotSliderChanged (int);
    void customLongStopToggled (bool);
    void customShortStopToggled (bool);
    void slotHelp ();
    void slotScaleToScreen (bool);
    void slotLogScaling (bool);

  protected:
    Config config;
    QTable *tradeList;
    QCheckBox *maximumLossCheck;
    QCheckBox *maximumLossLong;
    QCheckBox *maximumLossShort;
    QCheckBox *profitCheck;
    QCheckBox *profitLong;
    QCheckBox *profitShort;
    QCheckBox *trailingCheck;
    QCheckBox *trailingLong;
    QCheckBox *trailingShort;
    QCheckBox *tradeLong;
    QCheckBox *tradeShort;
    QCheckBox *customLongStopCheck;
    QCheckBox *customShortStopCheck;
    QLineEdit *maximumLossEdit;
    QLineEdit *profitEdit;
    QLineEdit *trailingEdit;
    SymbolButton *symbolButton;
    QSpinBox *entryCom;
    QSpinBox *exitCom;
    QSpinBox *account;
    QSpinBox *volumePercent;
    QSpinBox *margin;
    QSpinBox *tradeDelay;
    QSpinBox *bars;
    QDoubleValidator *validator;
    QString ruleName;
    QString chartType;
    QComboBox *priceField;
    QComboBox *compression;
    FormulaEdit *enterLongEdit;
    FormulaEdit *enterShortEdit;
    FormulaEdit *exitLongEdit;
    FormulaEdit *exitShortEdit;
    FormulaEdit *customShortStopEdit;
    FormulaEdit *customLongStopEdit;
//    Plot *plot;
    Plot *equityPlot;
    QSlider *slider;
    FuturesData fd;

    BarData *recordList;
    int currentRecord;
    double trailingHigh;
    double trailingLow;
    double equity;
    double volume;
    QStringList fieldList;
    QStringList compressionList;
//    PlotLine *equityCurve;
    PlotLine *customShortStopLine;
    PlotLine *customLongStopLine;

    // test
    QDict<Setting> enterLongSignal;
    QDict<Setting> exitLongSignal;
    QDict<Setting> enterShortSignal;
    QDict<Setting> exitShortSignal;
    QPtrList<Setting> trades;

    QLabel *summaryBalance;
    QLabel *summaryNetProfit;
    QLabel *summaryNetPercentage;
    QLabel *summaryInvestment;
    QLabel *summaryCommission;
    QLabel *summaryDrawdown;
    QLabel *summaryLongTrades;
    QLabel *summaryShortTrades;
    QLabel *summaryTrades;
    QLabel *summaryWinTrades;
    QLabel *summaryTotalWinTrades;
    QLabel *summaryAverageWin;
    QLabel *summaryLargestWin;
    QLabel *summaryWinLongTrades;
    QLabel *summaryWinShortTrades;
    QLabel *summaryLoseTrades;
    QLabel *summaryTotalLoseTrades;
    QLabel *summaryAverageLose;
    QLabel *summaryLargestLose;
    QLabel *summaryLoseLongTrades;
    QLabel *summaryLoseShortTrades;
    
    QSplitter *split;
};

#endif
