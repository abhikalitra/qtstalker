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

#ifndef PLOT_WIDGET_HPP
#define PLOT_WIDGET_HPP

#include <QWidget>
#include <QScrollBar>
#include <QSplitter>
#include <QHash>

#include "Plot.h"
#include "Widget.h"
#include "ControlWidget.h"
#include "DataBase.h"


class PlotWidget : public Widget
{
  Q_OBJECT
  
  signals:
    void signalClear ();
    void signalSetDates ();
    void signalDraw ();
    void signalBarLength (int);
    void signalIndex (int);

  public:
    PlotWidget ();
    ~PlotWidget ();
    void removePlot (QString);
    void setScrollBarSize ();
    void saveSettings ();
    void loadMarkers (DataBase &);
    void saveMarkers (DataBase &);
    void addPlotSettings (Entity *e);
    Entity * settings ();
    
  public slots:
    void addPlot (QString plugin, int row, QString name);
    void scrollBarChanged (int);
    void refresh ();
    void indicatorDialog ();
    void indicatorDialog2 (QString plugin, int row, QString name);
    void editIndicator ();
    void editIndicator2 (QString);
    void removeIndicator ();
    void removeIndicator2 (QString);
    void loadSettings ();
    void setBarLength ();
    void deleteMarkers (QStringList);
   
  private:
    QSplitter *_csplitter;
    QHash<QString, Plot *> _plots;
    QHash<QString, Entity *> _settings;
    ControlWidget *_cw;
    QString _removeName;
};

#endif
