/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#ifndef GROUPPAGE_HPP
#define GROUPPAGE_HPP

#include <qstring.h>
#include <qwidget.h>
#include <qpopupmenu.h>
#include <qlineedit.h>
#include "Navigator.h"
#include "Config.h"

class GroupPage : public QWidget
{
  Q_OBJECT

  signals:
    void fileSelected (QString);

  public:
    GroupPage (QWidget *);
    ~GroupPage ();

  public slots:
    void newGroup ();
    void addGroupItem ();
    void deleteGroupItem ();
    void deleteGroup ();
    void renameGroup ();
    void groupSelected (QString);
    void groupNoSelection ();
    void rightClick (QListBoxItem *);
    void editChart ();
    void refreshList ();
    void slotHelp ();

  protected:
    Navigator *nav;
    Config config;
    QPopupMenu *menu;
    QLineEdit *group;
};

#endif
