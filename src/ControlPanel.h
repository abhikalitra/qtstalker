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

#ifndef CONTROL_PANEL_HPP
#define CONTROL_PANEL_HPP

#include <QString>
#include <QWidget>
#include <QSlider>
#include <QToolButton>

#include "RecentCharts.h"
#include "BarLengthButton.h"
#include "BarSpaceButton.h"
#include "DateRangeControl.h"
#include "DataWindowButton.h"
#include "GridButton.h"
#include "CrossHairsButton.h"
#include "RefreshButton.h"
#include "ConfigureButton.h"

class ControlPanel : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalValueChanged(int);
    void signalLockStatus (bool);
    
  public:
    ControlPanel ();
    void createGUI ();
    void setStart (int, int, int);
    int getValue ();
    BarSpaceButton * barSpaceButton ();
    BarLengthButton * barLengthButton ();
    DateRangeControl * dateRangeControl ();
    RecentCharts * recentCharts ();
    RefreshButton * refreshButton ();
    ConfigureButton * configureButton ();
    QSlider * slider ();
    
  public slots:
    void setValue (int);
    void setStartValue (int);
    void sliderChanged (int);
    void setLockStatus (bool);
    void loadSettings ();
    void saveSettings ();
    
  protected:
    QSlider *_slider;
    QToolButton *_Button;
    bool _lockStatus;
    RecentCharts *_recentCharts;
    BarLengthButton *_barLengthButton;
    BarSpaceButton *_barSpaceButton;
    DateRangeControl *_dateRangeControl;
    GridButton *_gridButton;
    RefreshButton *_refreshButton;
    CrossHairsButton *_crossHairsButton;
    DataWindowButton *_dataWindowButton;
    ConfigureButton *_configureButton;
    int _startFlag;
};

#endif
