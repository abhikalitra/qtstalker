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

#ifndef TESTER_HPP
#define TESTER_HPP

#include <QString>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QList>
#include <QHash>
#include <QDialog>
#include "Setting.h"
#include "BarData.h"
#include "TesterReport.h"
#include "TradeItem.h"
#include "TesterRulePage.h"
#include "TesterTestPage.h"
#include "TesterStopPage.h"
#include "TesterChartPage.h"


class Tester : public QDialog
{
  Q_OBJECT
  
  signals:
    void message (QString);

  public:
    Tester (QString);
    Tester ();
    ~Tester ();
    int getVolume (int, double);
    double getPrice (int);
    QString newTest ();
    void loadSignals ();
    void enterTrade (TradeItem::TradePosition);

  public slots:
    void test ();
    void saveRule ();
    void exitDialog ();
    void loadRule ();
    void buttonPressed (QAbstractButton *);

  protected:
    TesterReport *reportPage;
    TesterRulePage *rulePage;
    TesterTestPage *testPage;
    TesterStopPage *stopPage;
    TesterChartPage *chartPage;
    QString ruleName;
    QString chartType;
    QString futuresType;
    BarData *recordList;
    int currentRecord;
    double equity;
    double volume;
    QHash<QString, Setting *> enterLongSignal;
    QHash<QString, Setting *> exitLongSignal;
    QHash<QString, Setting *> enterShortSignal;
    QHash<QString, Setting *> exitShortSignal;
    QList<TradeItem*> trades;
    QTabWidget *tabs;
    QDialogButtonBox *buttonBox;    
};

#endif