/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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
#include <qlistview.h>
#include <qdict.h>
#include <qlist.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qslider.h>
#include <qtable.h>
#include <qtoolbutton.h>
#include "Config.h"
#include "Indicator.h"
#include "Setting.h"
#include "ChartDb.h"
#include "BarData.h"

class Tester : public QTabDialog
{
  Q_OBJECT

  public:
    Tester (Config *, QString);
    ~Tester ();
    void createFormulaPage();
    void createStopPage();
    void createTestPage();
    void createReportPage();
    void exitPosition (QString);
    bool maximumLoss ();
    bool profit ();
    bool trailing ();
    void loadIndicators (int);
    void enterLong ();
    void exitLong ();
    void enterShort ();
    void exitShort ();
    void loadEnterLongAlerts ();
    void loadExitLongAlerts ();
    void loadEnterShortAlerts ();
    void loadExitShortAlerts ();
    void clearAlertCounts ();
    void checkAlerts ();
    void createSummary ();
    void getVolume ();

  public slots:
    void editIndicator ();
    void deleteIndicator ();
    void addIndicator ();
    void indicatorSelected (QListViewItem *);
    void test ();
    void maximumLossToggled (bool);
    void profitToggled (bool);
    void trailingToggled (bool);
    void symbolButtonPressed ();
    void showRule (int);
    void saveRule ();
    void exitDialog ();
    void loadRule ();

  protected:
    Config *config;
    QListView *indicatorList;
    QTable *tradeList;
    QListViewItem *item;
    QDict<Indicator> enterLongIndicators;
    QDict<Indicator> exitLongIndicators;
    QDict<Indicator> enterShortIndicators;
    QDict<Indicator> exitShortIndicators;
    QDict<Setting> enterLongAlerts;
    QDict<Setting> exitLongAlerts;
    QDict<Setting> enterShortAlerts;
    QDict<Setting> exitShortAlerts;
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
    QLineEdit *maximumLossEdit;
    QLineEdit *profitEdit;
    QLineEdit *trailingEdit;
    QPushButton *testButton;
    QPushButton *symbolButton;
    QSpinBox *entryCom;
    QSpinBox *exitCom;
    QSpinBox *account;
    QSpinBox *volumePercent;
    QSpinBox *margin;
    QDateEdit *startDate;
    QDateEdit *endDate;
    QDoubleValidator *validator;
    QButtonGroup *buttonGroup;
    QString ruleName;
    QToolButton *addIndicatorButton;
    QToolButton *editIndicatorButton;
    QToolButton *deleteIndicatorButton;

    int status;
    int testLoop;
    ChartDb *db;
    BarData *recordList;
    int currentRecord;
    int buyRecord;
    double trailingHigh;
    double trailingLow;
    double equity;
    int enterLongCount;
    int exitLongCount;
    int enterShortCount;
    int exitShortCount;
    int volume;

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
};

#endif
