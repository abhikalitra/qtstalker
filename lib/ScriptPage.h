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
#include <QAction>
#include <QHash>
#include <QTimer>
#include <QLayout>

#include "Script.h"
#include "ScriptLaunchButton.h"

class ScriptPage : public QWidget
{
  Q_OBJECT

//  signals:
//    void signalMessage (QString);

  public:
    enum Action
    {
      _LAUNCH_BUTTON_NEW,
      _SCRIPT_CANCEL,
      _SCRIPT_DELETE,
      _SCRIPT_EDIT,
      _SCRIPT_NEW,
      _SCRIPT_RUN,
      _SCRIPT_SEARCH,
      _SCRIPT_SHOW_ALL
    };

    ScriptPage (QWidget *);
    void createGUI ();
    void createActions ();
    void startScript ();
    void createButtonMenu ();
    void createMainPage ();
    QListWidget * list ();
    void runScriptSystem (QString);
    void selected (QStringList &);

  public slots:
    void queRightClick (const QPoint &);
    void searchRightClick (const QPoint &);
    void queStatus ();
    void searchStatus ();
    void runScriptDialog ();
    void runScriptInternal (QString name);
    void runScriptExternal (QString file, QString command);
    void done (QString);
    void cancel ();
    void newScript ();
    void editScript ();
    void deleteScript ();
    void newLaunchButton ();
    void newLaunchButton2 (QString script, QString icon, bool flag, int row, int col, QString text);
    void deleteLaunchButton (QString);
    void runStartupScripts ();
    void scriptThreadMessage (QString);
    void runIntervalScripts ();
    void updateSearchList ();
    void searchDialog ();
    void showAll ();
    void setSearchList (QStringList);
    void searchDoubleClicked (QListWidgetItem *);

  protected:
    QListWidget *_queList;
    QListWidget *_searchList;
    QMenu *_queMenu;
    QMenu *_searchMenu;
    QHash<int, QAction *> _actions;
    QHash<QString, QListWidgetItem *> _itemList;
    QHash<QString, Script *> _scripts;
    QTimer _timer;
    QHash<QString, ScriptLaunchButton *> _launchButtons;
    QGridLayout *_buttonGrid;
};

#endif
