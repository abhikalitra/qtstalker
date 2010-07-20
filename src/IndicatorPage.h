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

#ifndef INDICATOR_PAGE_HPP
#define INDICATOR_PAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QKeyEvent>
#include <QList>
#include <QAction>
#include <QToolButton>
#include <QHash>
#include <QToolBar>

#include "Setting.h"


class IndicatorPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalEnableIndicator (QString);
    void signalDeleteIndicator (QString);
    void signalNewIndicator (QString);
    void signalRefreshIndicator (QString);
    void signalMessage (QString);

  public:
    enum Action
    {
      ShowActive,
      ShowAll,
      Search,
      NewIndicator,
      EditIndicator,
      DeleteIndicator
    };

    IndicatorPage ();
    void createActions ();
    void createButtonMenu (QToolBar *);

  public slots:
    void doubleClick (QListWidgetItem *);
    void rightClick (const QPoint &);
    void newIndicator ();
    void editIndicator ();
    void editIndicator (QString &);
    void deleteIndicator ();
    void doKeyPress (QKeyEvent *key);
    void indicatorSearch ();
    void showAll ();
    void showActive ();
    void listStatus ();

  protected:
    virtual void keyPressEvent (QKeyEvent *);

    int listFlag;
    QListWidget *list;
    QMenu *menu;
    QHash<int, QAction *> actions;
};

#endif

