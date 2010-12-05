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
#include "PlotSettings.h"
#include "ScriptPluginFactory.h"

class ChartLayout : public QSplitter
{
  Q_OBJECT

  signals:
    void signalDraw ();
    void signalIndex (int);
    void signalClear ();
    void signalSaveSettings ();
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalFont (QFont);
    void signalMessage (QString);
    void signalGrid (bool);
    void signalNewChartObject (int);
//    void signalZoom (int, int);
    void signalInfo (Setting);
    void signalCrossHairsColor (QColor);
    void signalCrossHairs (bool);
    void signalBarSpacing (int);

  public:
    ChartLayout ();
    void addTab (Indicator *);
    void addTab (QString &);
    void refresh (QString);
    void loadPlots (int index);
    int plotWidth ();
    QHash<QString, PlotSettings> & plots ();

  public slots:
    void save ();
    void load ();
    void setGridColor (QColor);
    void setBarSpacing (int);
    void setIndex (int);
    void clearIndicator ();
    void saveSettings ();
    void setGrid (bool);
    void setZoom (int, int);
//    void editIndicator (QString);
//    void editIndicator2 (Indicator);
    void deleteIndicator ();
    void newIndicator ();
    void newIndicator2 (QStringList);
    void indicatorScriptFinished (QString);
    void backgroundColorChanged (QColor);
    void fontChanged (QFont);
    void removeTab (QStringList);
    void setCrossHairs (bool);
    void setCrossHairsColor (QColor);

  private:
    QHash<QString, PlotSettings> _plots;
    QHash<QString, TabWidget *> _tabs;
    QHash<int, QHBoxLayout *> _grids;
    QHash<int, QWidget *> _holders;
    int _startIndex;
    int _barSpacing;
    ScriptPluginFactory _factory;
};

#endif


