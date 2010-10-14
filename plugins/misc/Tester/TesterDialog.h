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

#ifndef TESTER_DIALOG_HPP
#define TESTER_DIALOG_HPP

#include <QDialog>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QTreeWidget>
#include <QHash>

#include "TesterThread.h"

class TesterDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
  
  public:
    TesterDialog ();
    void createTesterPage ();
    void createReportPage ();
    void loadSettings ();
    void saveSettings ();
    
  public slots:
    void runTest ();
    void runTest2 (QStringList);
    void newTest ();
    void editTest ();
    void deleteTest ();
    void closeDialog ();
    void done (QString);
    void stopTest ();
    void testStopped (QString);
    void updateReports ();
    void reportView ();
    void testSelectionChanged ();
    void reportSelectionChanged ();
          
  private:
    QTabWidget *_tabs;
    QListWidget *_testers;
    QPushButton *_stopButton;
    QPushButton *_viewButton;
    QTreeWidget *_reports;
    QHash<QString, QListWidgetItem *> _runningList;
    QHash<QString, TesterThread *> _threadList;
};

#endif
