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

#ifndef PLOT_MENU_HPP
#define PLOT_MENU_HPP

#include <QString>
#include <QMenu>
#include <QAction>
#include <QHash>

class PlotMenu : public QMenu
{
  Q_OBJECT

  signals:
    void signalNewChartObject (QString);
//    void signalDeleteAllChartObjects ();

  public:
    enum Action
    {
      _BUY_CHART_OBJECT,
      _HLINE_CHART_OBJECT,
      _RETRACEMENT_CHART_OBJECT,
      _SELL_CHART_OBJECT,
      _TEXT_CHART_OBJECT,
      _TLINE_CHART_OBJECT,
      _VLINE_CHART_OBJECT,
      _DELETE_ALL_CHART_OBJECTS,
      _ADD_INDICATOR,
      _REMOVE_INDICATOR
    };

    PlotMenu (QWidget *, QString);
    void createActions ();
    void createMenus ();
    void setCOMenuStatus (bool);

  public slots:
    void chartObjectMenuSelected (QAction *);
    void deleteAllChartObjects ();
    void addIndicator ();
    void removeIndicator ();

  private:
    QString _plotName;
    QMenu *_coListMenu;
    QHash<int, QAction *> _actions;
};

#endif
