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

#ifndef RECENT_CHARTS_HPP
#define RECENT_CHARTS_HPP

#include <QToolButton>
#include <QHash>
#include <QMenu>
#include <QList>
#include <QString>

class RecentCharts : public QToolButton
{
  Q_OBJECT

//  signals:
//    void signalChartSelected (QString);

  public:
    RecentCharts ();
    ~RecentCharts ();
    void createMenu ();

  public slots:
    void addRecentChart (QString);
    void itemSelected (QAction *);
    void save ();

  protected:
    int _maxCount;
    QMenu *_menu;
    QHash<QString, QString> _symbols;
    QList<QAction *> _actions;
};

#endif
