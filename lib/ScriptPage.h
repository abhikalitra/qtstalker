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
#include <QList>
#include <QAction>
#include <QToolButton>
#include <QHash>

#include "Script.h"

class ScriptPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    ScriptPage (QWidget *);
    ~ScriptPage ();
    void startScript ();
    void saveRunningScripts ();
    void loadSavedScripts ();

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

  protected:
    QListWidget *list;
    QListWidget *queList;
    QMenu *menu;
    QList<QAction *> actions;
    QToolButton *allButton;
    QToolButton *searchButton;
    QString searchString;
    QHash<QString, Script *> scripts;
};

#endif

