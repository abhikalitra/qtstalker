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

#ifndef GROUP_PAGE_HPP
#define GROUP_PAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QComboBox>
#include <QHash>
#include <QAction>

#include "SymbolListWidget.h"

class GroupPage : public QWidget
{
  Q_OBJECT

//  signals:

  public:
    enum Action
    {
      _NEW_GROUP,
      _ADD_GROUP,
      _EDIT_GROUP,
      _DELETE_GROUP
    };

    GroupPage (QWidget *);
    void createActions ();
    void createButtonMenu ();
    void selected (QStringList &);

  public slots:
    void updateList ();
    void newGroup ();
    void editDialog (QString);
    void editGroup ();
    void deleteGroup ();
    void groupSelected (int);
    void rightClick (const QPoint &);
    void chartOpened (QString);
    void updateGroups ();
    void addToGroup ();
    void loadGroups ();
    void selectionChanged ();
    void itemClicked (QListWidgetItem *);

  protected:
    SymbolListWidget *_nav;
    QMenu *_menu;
    QComboBox *_groups;
    QHash<int, QAction *> _actions;
};

#endif
