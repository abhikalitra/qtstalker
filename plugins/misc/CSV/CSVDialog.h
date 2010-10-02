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

#ifndef CSV_DIALOG_HPP
#define CSV_DIALOG_HPP

#include <QDialog>
#include <QTextEdit>
#include <QStringList>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHash>

#include "CSVThread.h"

class CSVDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalChartRefresh ();
    void signalMessage (QString);
  
  public:
    CSVDialog ();
    void createMainPage ();
    void createLogPage ();
    void loadSettings ();
    void saveSettings ();
    void updateRules ();
    
  public slots:
    void run ();
    void run2 (QStringList);
    void newRule ();
    void editRule ();
    void editRule2 (QStringList);
    void deleteRule ();
    void done (QString);
    void closeDialog ();
    void rulesSelectionChanged ();
    void stop ();
          
  private:
    QTextEdit *_log;
    QTabWidget *_tabs;
    QListWidget *_rules;
    QPushButton *_cancelButton;
    QHash<QString, CSVThread *> _threads;
};

#endif
