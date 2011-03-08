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
    void signalLockStatus (bool);
    void signalDateStatus (bool);
    void signalLogStatus (bool);
    void signalNewChartObject (QString);

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
      _NEW_INDICATOR,
      _EDIT_INDICATOR,
      _DELETE_INDICATOR,
      _DATE_AXIS,
      _LOG_SCALING,
      _LOCK
    };
    
    PlotMenu (QWidget *);
    void createActions ();
    void createMenus ();
    void setCOMenuStatus (bool);
    void setLog (bool);
    bool log ();
    void setDate (bool);
    bool date ();
    void setLock (bool);
    bool lock ();
    void setIndicator (QString);

  public slots:
    void chartObjectMenuSelected (QAction *);
    void editIndicator ();
    void deleteIndicator ();
    void deleteAllChartObjects ();
    void newIndicator ();

  private:
    QMenu *_coListMenu;
    QString _indicator;
    QHash<int, QAction *> _actions;
};

#endif
