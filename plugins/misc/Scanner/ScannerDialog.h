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

#ifndef SCANNER_DIALOG_HPP
#define SCANNER_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QTabWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHash>

#include "ScannerItem.h"
#include "ScannerThread.h"

class ScannerDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalGroupRefresh ();
    void signalMessage (QString);
  
  public:
    ScannerDialog ();
    void createMainPage ();
    void loadSettings ();
    void saveSettings ();
    
  public slots:
    void runScanner ();
    void runScanner2 (QStringList);
    void newScanner ();
    void editScanner ();
    void deleteScanner ();
    void done (QString);
    void closeDialog ();
    void selectionChanged ();
    void stopScanner ();
    void scannerStopped (QString);
          
  private:
    QTabWidget *_tabs;
    QListWidget *_scanners;
    QPushButton *_stopButton;
    QHash<QString, QListWidgetItem *> _runningList;
    QHash<QString, ScannerThread *> _threadList;
};

#endif
