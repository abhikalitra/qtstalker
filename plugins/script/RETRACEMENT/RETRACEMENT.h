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

#ifndef PLUGIN_RETRACEMENT_HPP
#define PLUGIN_RETRACEMENT_HPP

#include <QStringList>
#include <QPoint>
#include <QMenu>
#include <QHash>

#include "Plugin.h"
#include "Dialog.h"
#include "RetracementDraw.h"

class RETRACEMENT : public Plugin
{
  Q_OBJECT

  signals:
    void signalSelected ();
    void signalUnselected ();

  public:
    enum Status
    {
      _NONE,
      _SELECTED,
      _MOVE,
      _MOVE2
    };

    RETRACEMENT ();
    ~RETRACEMENT ();
    int command (Command *);
    int calculate (BarData *, Indicator *, Setting *);
    void defaults (Setting *);
    int request (Setting *, Setting *);
    void setParent (void *);

    int clear ();
    int addItem (Setting *);
    int info (Setting *);
    int highLow (Setting *, Setting *);
    int create (Setting *);
    void load ();
    int deleteAll ();
    void update ();
    void save ();

  public slots:
    void move (QPoint);
    void click (int, QPoint);
    void dialog ();
    void dialogCancel ();
    void dialogOK ();
    void deleteChartObject ();
    void deleteChartObject2 ();

  private:
    QwtPlot *_plot;
    Dialog *_dialog;
    QHash<QString, RetracementDraw *> _items;
    Status _status;
    QMenu *_menu;
    QAction *_editAction;
    QAction *_deleteAction;
    RetracementDraw *_selected;
    int _createFlag;
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
