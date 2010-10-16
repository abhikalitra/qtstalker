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

#ifndef INDICATOR_PLOT_LIST_HPP
#define INDICATOR_PLOT_LIST_HPP

#include <QWidget>
#include <QLabel>
#include <QAction>
#include <QTreeWidget>
#include <QStringList>

#include "Setting.h"
#include "Indicator.h"

class IndicatorPlotList : public QWidget
{
  Q_OBJECT

  signals:
    void signalIndicatorChanged ();
    void signalItemChanged ();

  public:
    IndicatorPlotList ();
    void buttonStatus ();

  public slots:
    void selectIndicator ();
    void selectIndicator2 (QString);
    void editIndicator ();
    void editIndicator2 (Indicator);
    void addPlot ();
    void addPlot2 (QString);
    void deletePlot ();
//    void deletePlot2 (QString);
    void indicatorChanged ();
    void addPlotItem (QString pn, QString oper, QString val);
    void setIndicator (QString);
    QString indicator ();
    void setList (QStringList l);
    void list (QStringList &items);
    void setSettings (Setting);
    Setting settings ();

  private:
    QLabel *_indicator;
    QTreeWidget *_list;
    QAction *_indicatorAction;
    QAction *_indicatorSettingsAction;
    QAction *_addAction;
    QAction *_deleteAction;
    Setting _settings;
    QStringList _plotNames;
};

#endif


