/*
 *  CSV stock quote importer
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "CSV.h"

#include <QMainWindow>
#include <QHash>
#include <QAction>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTextEdit>
#include <QCloseEvent>




class MainWindow : public QMainWindow
{
  Q_OBJECT

  public:
    enum MenuAction
    {
      NewRule,
      OpenRule,
      EditRule,
      DeleteRule,
      Exit,
      About,
      DumpIndex,
      DumpSymbol
    };

    MainWindow();
    ~MainWindow();
    void createActions ();
    void createMenus ();
    void createToolBars ();
    void createStatusBar ();
    void loadSettings ();
    void saveSettings ();
    void setConfig (QString &k, QString &d);
    void getConfig (QString &k, QString &d);
    void loadRules ();
    void loadRule (CSVRule &rule);
    void addRule (QString &name);

  public slots:
    void newRule ();
    void editRule ();
    void deleteRule ();
    void about ();
    void getRules (QStringList &);
    void itemDoubleClicked (QTreeWidgetItem *, int);
    void statusChanged ();
    void message (QString);
    void CSVActive (QString);
    void CSVInactive (QString);
    void dumpIndex ();
    void dumpSymbol ();

  protected:
    void closeEvent(QCloseEvent *event);

  private:
    QHash<MenuAction, QAction *> actionList;
    QHash<QString, CSV *> CSVList;
    QHash<QString, QTreeWidgetItem *> itemList;
    QTreeWidget *ruleList;
    QTextEdit *log;
};

#endif

