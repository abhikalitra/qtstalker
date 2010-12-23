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

#ifndef CHART_LAYOUT_HPP
#define CHART_LAYOUT_HPP

#include <QSplitter>
#include <QHash>
#include <QString>
#include <QColor>
#include <QFont>
#include <QLayout>

#include "Indicator.h"
#include "Setting.h"
#include "TabWidget.h"
#include "ScriptPluginFactory.h"
#include "Plot.h"

class ChartLayout : public QSplitter
{
  Q_OBJECT

  signals:
    void signalDraw ();
    void signalIndex (int);
    void signalClear ();
    void signalSaveSettings ();
    void signalMessage (QString);
    void signalNewChartObject (int);
    void signalInfo (Setting);
    void signalBarSpacing (int);

  public:
    ChartLayout ();
    void addTab (Indicator *);
    void addTab (QString);
    void refresh (QString);
    void loadPlots (int index);
    int plotWidth ();
    void loadPlot (QString name);

  public slots:
    void save ();
    void load ();
    void setBarSpacing (int);
    void setIndex (int);
    void clearIndicator ();
    void saveSettings ();
    void setZoom (int, int);
    void indicatorScriptFinished (QString);
    void removeTab (QStringList);
    void addNewTab (QString);

  private:
    QHash<QString, TabWidget *> _tabs;
    QHash<int, QHBoxLayout *> _grids;
    QHash<int, QWidget *> _holders;
    QHash<QString, Indicator *> _indicators;
    QHash<QString, Plot *> _plots;
    int _startIndex;
    int _barSpacing;
    ScriptPluginFactory _factory;
};

#endif


