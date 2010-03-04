/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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

#ifndef PLOT_HPP
#define PLOT_HPP

#include <QWidget>
#include <QString>
#include <QColor>
#include <QFont>
#include <QStringList>

#include "Scaler.h"
#include "BarData.h"
#include "DatePlot.h"
#include "ScalePlot.h"
#include "IndicatorPlot.h"


class Plot : public QWidget
{
  Q_OBJECT

  public:
    Plot (QWidget *);
    void setData (BarData *);
    void setLogScale (bool);
    void updateStatusBar (int, int);
    void setInfoFlag (bool);
    void drawCrossHair ();
    int getWidth ();
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setDateFlag (bool);
    void setCrosshairsFlag (bool);
    IndicatorPlot * getIndicatorPlot ();
    DatePlot * getDatePlot ();
    void setMenuFlag (bool);

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void setInterval(BarData::BarLength);
    void crossHair (int, int, bool);
    void setChartPath (QString);
    void setCrosshairsStatus (bool);
    void slotSliderChanged (int);
    void slotGridChanged (bool);
    void slotScaleToScreenChanged (bool);
    void slotDateFlagChanged (bool);
    void slotLogScaleChanged (bool);
    void slotUpdateScalePlot ();
    void clear ();
    void loadChartObjects ();

  private:
    DatePlot *datePlot;
    ScalePlot *scalePlot;
    IndicatorPlot *indicatorPlot;
};

#endif


