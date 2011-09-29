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

#include "ScriptTimer.h"
#include "Script.h"

class ScriptPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum Action
    {
      _RunScript,
      _Cancel,
      _LaunchButtonRows,
      _LaunchButtonCols,
      _NewScriptTimer,
      _EditScriptTimer,
      _DeleteScriptTimer
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
    void runScript (QString file, QString command);
    void done (QString);
    void cancel ();
    void launchButtonRows ();
    void launchButtonRows2 (int);
    void launchButtonCols ();
    void launchButtonCols2 (int);
    void setupScriptTimers ();
    void shutDown ();
    void newScriptTimer ();
    void editScriptTimer ();
    void editScriptTimer (QString);
    void editScriptTimer2 (QStringList);
    void editScriptTimer3 (QString);
    void deleteScriptTimer ();
    void deleteScriptTimer2 (QStringList);
    void runStartupScripts ();
    void addScriptTimer (QString name, QString file, QString interval, QString command);
    void scriptMessage (Data *);

  protected:
    QListWidget *_queList;
    QMenu *_queMenu;
    QHash<int, QAction *> _actions;
    QHash<QString, QListWidgetItem *> _itemList;
    QHash<QString, Script *> _scripts;
    QHash<QString, ScriptTimer *> _timers;
};

#endif
