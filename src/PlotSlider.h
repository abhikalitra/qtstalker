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

#ifndef PLOT_SLIDER_HPP
#define PLOT_SLIDER_HPP

#include <QString>
#include <QWidget>
#include <QSlider>
#include <QList>

#include "ZoomButtons.h"

class PlotSlider : public QWidget
{
  Q_OBJECT
  
  signals:
    void signalValueChanged(int);
    
  public:
    PlotSlider ();
    void setStart (int, int, ZoomButtons *);
    int getValue ();
    
  public slots:
    void setValue (int);
    void startButtonClicked ();
    void pPageButtonClicked ();
    void pBarButtonClicked ();
    void nBarButtonClicked ();
    void nPageButtonClicked ();
    void endButtonClicked ();
    
  protected:
    QSlider *slider;
};

#endif
