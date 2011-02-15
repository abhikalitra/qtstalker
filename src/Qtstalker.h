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

#ifndef QTSTALKER_H
#define QTSTALKER_H

#include <QMainWindow>
#include <QToolBar>
#include <QStringList>
#include <QHash>

#include "InfoPanel.h"
#include "SidePanel.h"
#include "Command.h"
#include "Plot.h"
#include "ControlPanel.h"

class QtstalkerApp : public QMainWindow
{
  Q_OBJECT

  signals:
    void signalLoadSettings ();
    void signalClearPlot ();
    void signalPlot ();
    void signalShutDown ();

  public:
    QtstalkerApp (QString session, QString asset);
    void createGUI ();
    void loadSettings ();
    QString getWindowCaption ();
    void setSliderStart (int);
    void addPlot (QString);

  public slots:
    void loadChart (BarData);
    void save ();
    void chartUpdated ();
    void statusMessage (QString);
    void wakeup ();
    void commandLineAsset ();
    void addNewPlot (QString);
    void deletePlot (QStringList);
    void setPlotTabPosition (int);
    void fixDockTabs ();
    void updatePlot (QString);
    void shutDown ();

  protected:
    QHash<QString, Plot *> _plots;
    InfoPanel *_infoPanel;
    QString _clAsset;
    SidePanel *_sidePanel;
    ControlPanel *_controlPanel;
};

#endif
