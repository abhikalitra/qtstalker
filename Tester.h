/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001 Stefan S. Stratigakos
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
#include <qdialog.h>
#include <qtabwidget.h>
#include <qlistview.h>
#include <qdict.h>
#include <qlist.h>
#include <qtoolbutton.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qspinbox.h>
#include <qdatetimeedit.h>
#include <qlineedit.h>
#include <qvalidator.h>
#include "Config.h"
#include "Indicator.h"
#include "Setting.h"

class Tester : public QDialog
{
  Q_OBJECT

  public:
    Tester (Config *);
    ~Tester ();
    void createFormulaPage();
    void createStopPage();
    void createTestPage();
    void createReportPage();
    bool checkPosition (int, int, int);
    void exitPosition (int, Setting *, Setting *, QString);
    bool breakeven (int, Setting *, Setting *);
    bool maximumLoss (int, Setting *, Setting *);
    bool profit (int, Setting *, Setting *);
    bool trailing (int, Setting *, double);

  public slots:
    void editIndicator ();
    void deleteIndicator ();
    void addIndicator ();
    void indicatorSelected (QListViewItem *);
    void test ();
    void breakevenToggled (bool);
    void maximumLossToggled (bool);
    void profitToggled (bool);
    void trailingToggled (bool);
    void symbolButtonPressed ();

  protected:
    Config *config;
    QTabWidget *tabs;
    QListView *ruleList;
    QListView *tradeList;
    QListViewItem *item;
    QList<QListViewItem> baseItemList;
    QDict<Indicator> indicators;
    QToolButton *addIndicatorButton;
    QToolButton *editIndicatorButton;
    QToolButton *deleteIndicatorButton;
    QCheckBox *breakevenCheck;
    QCheckBox *breakevenLong;
    QCheckBox *breakevenShort;
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
    QSpinBox *delayDays;
    QSpinBox *entryCom;
    QSpinBox *exitCom;
    QSpinBox *account;
    QDateEdit *startDate;
    QDateEdit *endDate;
    QDoubleValidator *validator;
};

#endif
