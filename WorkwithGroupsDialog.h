/*
 *  Qtstalker stock charter
 *
 *  Copyright (c) 2001,2002 Stefan S. Stratigakos
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

#ifndef WORKWITHGROUPSDIALOG_HPP
#define WORKWITHGROUPSDIALOG_HPP

#include <qstring.h>
#include <qtoolbutton.h>
#include <qlistview.h>
#include <qlayout.h>
#include <qdialog.h>
#include "Config.h"

class WorkwithGroupsDialog : public QDialog
{
  Q_OBJECT
  
  signals:
    void groupChanged ();

  public:
    WorkwithGroupsDialog (Config *);
    ~WorkwithGroupsDialog ();
    void updateList ();

  public slots:
    void renameGroup ();
    void newGroup ();
    void deleteGroup ();
    void editGroup ();
    void groupSelected (QListViewItem *);

  private:
    QListView *list;
    QListViewItem *item;
    Config *config;
    QToolButton *cancelButton;
    QToolButton *editButton;
    QToolButton *deleteButton;
    QToolButton *newButton;
    QToolButton *renameButton;
    QGridLayout *toolbar;
};

#endif
