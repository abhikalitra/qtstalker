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

#include "Plot.h"
#include "Indicator.h"
#include "Setting.h"
#include "BarData.h"
#include "TabWidget.h"

class ChartLayout : public QSplitter
{
  Q_OBJECT

  signals:
    void signalBackgroundColor (QColor);
    void signalBorderColor (QColor);
    void signalGridColor (QColor);
    void signalPlotFont (QFont);
    void signalZoom (int, int);
    void signalInfo (Setting *);
    void signalStatus (QString);
    void signalPixelSpace (int);
    void signalIndex (int);
    void signalInterval (BarData::BarLength);
    void signalClearIndicator ();
    void signalSaveSettings ();
    void signalGrid (bool);
    void signalNewExternalChartObject (QString);
    void signalNewExternalChartObjectDone ();
    void signalSetExternalChartObject ();
    void signalCursorChanged (int);
    void signalIndexChanged (int);
    void signalRefresh ();

  public:
    ChartLayout ();
    void addTab (Indicator &);
    void addTab (QString &);
    void removeTab (Indicator &);
    void refresh (QString);
    void loadPlots (BarData &, int index);
    int plotWidth ();

  public slots:
    void save ();
    void load ();
    void drawTab (QWidget *);
    void drawPlot (QString d);
    void drawPlots ();
    void setGridColor (QColor);
    void setPixelSpace (int);
    void setIndex (int);
    void setInterval (int);
    void clearIndicator ();
    void saveSettings ();
    void setGrid (bool);
    void newExternalChartObject (QString);
    void newExternalChartObjectDone ();
    void setExternalChartObject ();
    void cursorChanged (int);
    void setZoom (int, int);
    void newIndicator ();
    void editIndicator (QString);
    void deleteIndicator (QString);
    void moveIndicator (QString);

  private:
    QHash<QString, Plot *> _plotList;
    QHash<QString, TabWidget *> _tabs;
    QHash<int, QGridLayout *> _grids;
    QHash<int, QWidget *> _holders;
};

#endif


