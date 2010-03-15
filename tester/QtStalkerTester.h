/*
 *  QtStalkerTester
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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
#include <QString>
#include <QList>

#include "BarData.h"
#include "TestTrade.h"
#include "TestRankings.h"
#include "TestSettings.h"
#include "TestReport.h"
#include "TestChart.h"

class QtStalkerTester : public QMainWindow
{
  Q_OBJECT

  public:
    enum Status
    {
      None,
      Long,
      Short,
      WaitShort,
      WaitLong
    };

    QtStalkerTester ();
    void createActions ();
    void createMenuBar ();
    void createToolBars ();
    void restoreSettings ();
    void loadTest (QString &);

  public slots:
    void about ();
    void quit ();
    void newTest ();
    void openTest ();
    void saveTest ();
    void run ();
    void cancelTest ();

  protected:
    void closeEvent(QCloseEvent *event);

  private:
    QString name;
    QAction *exitAction;
    QAction *aboutAction;
    QAction *newAction;
    QAction *openAction;
    QAction *saveAction;
    QAction *runAction;
    QStatusBar *statusbar;
    QTabWidget *tabs;
    TestRankings *rankings;
    TestSettings *settings;
    TestReport *report;
    TestChart *chart;
};

#endif

