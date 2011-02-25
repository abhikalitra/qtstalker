/*
 *  Qtstalker stock charter
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

#ifndef SCRIPT_PAGE_HPP
#define SCRIPT_PAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QKeyEvent>
#include <QAction>
#include <QToolButton>
#include <QHash>
#include <QToolBar>
#include <QTimer>

#include "Script.h"

class ScriptPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalChartRefresh ();
    void signalMessage (QString);
    void signalCancelScript (QString);

  public:
    enum Action
    {
      _RunScript,
      _RunExternalScript,
      _NewScript,
      _EditScript,
      _DeleteScript,
      _CancelScript,
      _LaunchButtonRows,
      _LaunchButtonCols
    };

    ScriptPage (QWidget *);
    void createGUI ();
    void createActions ();
    void startScript ();
    void createButtonMenu ();
    void createMainPage ();
    QListWidget * list ();

  public slots:
    void queRightClick (const QPoint &);
    void queStatus ();
    void runScript ();
    void runExternalScript (QString);
    void runScript (QString);
    void newScript ();
    void editScript ();
    void deleteScript ();
    void done (QString);
    void cancelScript ();
    void cancelScript (QString);
    void launchButtonRows ();
    void launchButtonRows2 (int);
    void launchButtonCols ();
    void launchButtonCols2 (int);
    void scriptTimer ();
    void fileSelect ();
    void setupScript (Script *);
    void shutDown ();

  protected:
    QListWidget *_queList;
    QMenu *_queMenu;
    QHash<int, QAction *> _actions;
    QHash<QString, QListWidgetItem *> _itemList;
    QHash<QString, Script *> _scriptList;
    QTimer _timer;
};

#endif

