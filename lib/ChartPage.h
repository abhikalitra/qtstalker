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

#ifndef CHART_PAGE_HPP
#define CHART_PAGE_HPP

#include <QStringList>
#include <QWidget>
#include <QMenu>
#include <QPoint>
#include <QHash>
#include <QAction>
#include <QListWidget>

#include "Symbol.h"

class ChartPage : public QWidget
{
  Q_OBJECT

  public:
    enum Action
    {
      _SHOW_ALL,
      _SEARCH,
      _ADD_GROUP,
      _DELETE,
      _SELECT_ALL,
      _UNSELECT_ALL
    };

    ChartPage (QWidget *);
    void createActions ();
    void createMenu ();
    void selected (QStringList &);
    void refresh ();

  public slots:
    void rightClick (const QPoint &);
    void chartOpened (QString);
    void addToGroup ();
    void symbolSearch ();
    void setSearch (QString symbol);
    void allButtonPressed ();
    void buttonStatus ();
    void deleteSymbol ();
    void itemClicked (QListWidgetItem *);
    void updateList ();

  protected:
    QString _searchString;
    QListWidget *_nav;
    QMenu *_menu;
    QHash<int, QAction *> _actions;
};

#endif
