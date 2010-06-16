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

#ifndef SCRIPTPAGE_HPP
#define SCRIPTPAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QKeyEvent>
#include <QAction>
#include <QToolButton>
#include <QHash>
#include <QToolBar>

#include "Script.h"

class ScriptPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalScriptDone ();

  public:
    enum Action
    {
      RunScript,
      ShowAllScripts,
      SearchScript,
      NewScript,
      EditScript,
      DeleteScript,
      CancelScript
    };

    ScriptPage (QWidget *);
    ~ScriptPage ();
    void createActions ();
    void startScript ();
    void createButtonMenu (QToolBar *);

  public slots:
    void newScript ();
    void editScript ();
    void editScript (QString &);
    void deleteScript ();
    void listDoubleClick (QListWidgetItem *);
    void listRightClick (const QPoint &);
    void queRightClick (const QPoint &);
    void search ();
    void showAllScripts ();
    void updateQueList ();
    void scriptDone (QString);
    void removeScriptQueue ();
    void runScript (Script *);
    void runScriptDialog ();
    void listStatus ();
    void queStatus ();
    void saveRunningScripts ();
    void loadSavedScripts ();

  protected:
    QListWidget *list;
    QListWidget *queList;
    QMenu *listMenu;
    QMenu *queMenu;
    QHash<int, QAction *> actions;
    QToolButton *allButton;
    QToolButton *searchButton;
    QString searchString;
    QHash<QString, Script *> scripts;
};

#endif

