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

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QKeyEvent>
#include <QPoint>
#include <QComboBox>
#include <QHash>
#include <QAction>
#include <QToolBar>

#include "BarData.h"
#include "SymbolListWidget.h"
#include "Script.h"

class ChartPage : public QWidget
{
  Q_OBJECT

  signals:
    void fileSelected (BarData);
    void signalAddToGroup ();
    void signalReloadChart ();
    void addRecentChart (BarData);
    void signalMessage (QString);

  public:
    enum Action
    {
      ShowAll,
      Search,
      AddGroup,
      Delete
    };

    ChartPage ();
    void createActions ();
    void createButtonMenu ();
    void selected (QStringList &);

  public slots:
    void rightClick (const QPoint &);
    void doKeyPress (QKeyEvent *);
    void chartOpened (BarData);
    void addToGroup ();
    void updateList ();
    void symbolSearch ();
    void setSearch (QString exchange, QString symbol);
    void allButtonPressed ();
    void buttonStatus ();

  protected:
    QString _searchString;
    QString _searchExchange;
    SymbolListWidget *_nav;
    QMenu *_menu;
    QHash<int, QAction *> _actions;
    Command _symbolDialogCommand;
    Command _addGroupCommand;
};

#endif

