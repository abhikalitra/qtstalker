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

#ifndef SCRIPT_DIALOG_HPP
#define SCRIPT_DIALOG_HPP

#include <QDialog>
#include <QStringList>
#include <QTreeWidget>
#include <QHash>
#include <QTextEdit>
#include <QTabWidget>

#include "ScriptItem.h"

class ScriptDialog : public QDialog
{
  Q_OBJECT

  signals:
    void signalChartRefresh ();
    void signalMessage (QString);
  
  public:
    ScriptDialog ();
    void createMainPage ();
    void createLogPage ();
    void loadSettings ();
    void saveSettings ();
    void resizeColumns ();
    
  public slots:
    void runScript ();
    void runScript2 (QStringList);
    void newScript ();
    void editScript ();
    void editScript2 (QStringList);
    void deleteScript ();
    void deleteScript2 (QStringList);
    void done (QString);
    void closeDialog ();
    void cancelScript ();
    void cancelScript2 (QStringList);
          
  private:
    QTabWidget *_tabs;
    QTextEdit *_log;
    QTreeWidget *_list;
    QHash<QString, QTreeWidgetItem *> _running;
    QHash<QString, ScriptItem *> _scripts;
};

#endif
