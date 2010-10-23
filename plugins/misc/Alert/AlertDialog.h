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

#ifndef ALERT_DIALOG_HPP
#define ALERT_DIALOG_HPP

#include <QStringList>
#include <QTreeWidget>
#include <QPushButton>
#include <QHash>
#include <QTimer>
#include <QLabel>

#include "AlertItem.h"
#include "Dialog.h"

class AlertDialog : public Dialog
{
  Q_OBJECT

  signals:
    void signalChartRefresh ();
    void signalMessage (QString);
  
  public:
    AlertDialog ();
    void createMainPage ();
    void loadSettings ();
    void saveSettings ();
    void loadAlerts ();
    void resizeColumns ();
    
  public slots:
    void run ();
    void newAlert ();
    void newAlert2 (AlertItem);
    void editAlert ();
    void editAlert2 (AlertItem);
    void deleteAlert ();
    void done (AlertItem);
    void closeDialog ();
    void selectionChanged ();
    void resetAlert ();
    void configureDialog ();
    void enableChanged (int);
    void intervalChanged (int);
    void confirmYes ();
    void confirmNo ();
          
  private:
    QTreeWidget *_alerts;
    QPushButton *_editButton;
    QPushButton *_deleteButton;
    QPushButton *_resetButton;
    QHash<int, QTreeWidgetItem *> _treeItems;
    QTimer _timer;
};

#endif
