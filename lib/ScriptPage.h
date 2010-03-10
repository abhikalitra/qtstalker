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

#ifndef SCRIPTPAGE_HPP
#define SCRIPTPAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QKeyEvent>
#include <QList>
#include <QAction>
#include <QToolButton>

#include "ExScript.h"
#include "Script.h"


class ScriptPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum Status
    {
      StatusAll,
      StatusQueue,
      StatusSearch
    };

    ScriptPage (QWidget *);
    ~ScriptPage ();
    void startScript ();

  public slots:
    void doubleClick (QListWidgetItem *);
    void rightClick (const QPoint &);
    void newScript ();
    void editScript ();
    void editScript (QString &);
    void deleteScript ();
    void search ();
    void update ();
    void showAllScripts ();
    void showQueue ();
    void showSearch ();
    void addScriptQueue (Script &);
    void scriptDone ();
    void removeScriptQueue ();

  protected:
    ExScript *scriptServer;
    Status status;
    QListWidget *list;
    QMenu *menu;
    QList<QAction *> actions;
    QToolButton *allButton;
    QToolButton *queueButton;
    QToolButton *searchButton;
    QList<Script> scriptList;
    QString searchString;
};

#endif

