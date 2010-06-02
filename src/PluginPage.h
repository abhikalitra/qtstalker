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

#ifndef PLUGIN_PAGE_HPP
#define PLUGIN_PAGE_HPP

#include <QString>
#include <QWidget>
#include <QMenu>
#include <QListWidget>
#include <QList>
#include <QAction>
#include <QToolButton>
#include <QHash>
#include <QToolBar>

#include "MiscPluginFactory.h"

class PluginPage : public QWidget
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum Action
    {
      Configure
    };

    PluginPage (QWidget *);
    void createActions ();
    void createButtonMenu (QToolBar *);

  public slots:
    void doubleClick (QListWidgetItem *);
    void rightClick (const QPoint &);
    void configure ();
    void configure (QString &);
    void updateList ();
    void listStatus ();

  protected:
    QListWidget *_list;
    QMenu *_menu;
    QHash<int, QAction *> _actions;
    MiscPluginFactory _fac;
    QString _pluginPath;
};

#endif

