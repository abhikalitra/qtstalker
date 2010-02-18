/*
 *  QtStalkerTester
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



#ifndef QTSTALKERTESTER_H
#define QTSTALKERTESTER_H

#include <QMainWindow>
#include <QAction>
#include <QStatusBar>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QTextEdit>
#include <QStringList>
#include <QString>
#include <QLabel>
#include <QDialogButtonBox>
#include <QHash>
#include <QSpinBox>
#include <QList>
#include <QPushButton>
#include <QTreeWidget>

#include "Test.h"
#include "BarData.h"
#include "PlotLine.h"
#include "TestSignal.h"
#include "TestTrade.h"

class QtStalkerTester : public QMainWindow
{
  Q_OBJECT

  signals:

  public:
    QtStalkerTester ();
    ~QtStalkerTester ();
    void createActions ();
    void createMenuBar ();
    void createToolBars ();
    void restoreSettings ();
    void createSummaryTab();
    void createRuleTab();
    void createStopsTab();
    void createReportTab();
    void createRankTab();
    void loadTest (QString &);
    void getSettings ();
    void createSignals (BarData *, PlotLine *line, QHash<int, TestSignal *> &);
    void startTrade (int pos, BarData *data, TestTrade *trade);
    void endTrade (int pos, BarData *data, TestTrade *trade);
    void createSummary (QList<TestTrade *> &trades);
    void runTrades (BarData *data, QList<TestTrade *> &trades);
    void createTrades (BarData *data, QList<TestTrade *> &trades, int type,
		       QHash<int, TestSignal *> &enterSigs, QHash<int, TestSignal *> &exitSigs);
    void updateRankings ();

  public slots:
    void about ();
    void quit ();
    void newTest ();
    void openTest ();
    void saveTest ();
    void scriptButtonPressed ();
    void symbolButtonPressed ();
    void run ();
    void cancelTest ();

  protected:
    void closeEvent(QCloseEvent *event);

  private:
    Test test;
    QStringList priceList;
    QStringList barLengthList;
    QString scriptFile;

    QAction *exitAction;
    QAction *aboutAction;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;

    QWidget *baseWidget;
    QStatusBar *statusbar;
    QTabWidget *tabs;
    QLineEdit *symbol;
    QLineEdit *script;
    QComboBox *enterField;
    QComboBox *exitField;
    QCheckBox *trailingCheck;
    QDoubleSpinBox *trailing;
    QTextEdit *tradeLog;
    QDialogButtonBox *buttonBox;
    QSpinBox *bars;
    QSpinBox *delay;
    QDoubleSpinBox *entryComm;
    QDoubleSpinBox *exitComm;
    QComboBox *barLength;
    QPushButton *scriptButton;
    QDoubleSpinBox *account;
    QDoubleSpinBox *volumePercentage;
    QTreeWidget *rankTree;

    // summary stuff
    QLabel *grossProfit;
    QLabel *netProfit;
    QLabel *maxDrawDown;
    QLabel *avgDrawDown;
    QLabel *commissions;
    QLabel *winLossRatio;
    QLabel *totalTrades;
    QLabel *percentProfitable;
    QLabel *winningTrades;
    QLabel *losingTrades;
    QLabel *maxWinTrade;
    QLabel *maxLossTrade;
    QLabel *avgWinTrade;
    QLabel *avgLossTrade;
    QLabel *maxWinLong;
    QLabel *maxLossLong;
    QLabel *maxWinShort;
    QLabel *maxLossShort;
    QLabel *balance;
};

#endif

