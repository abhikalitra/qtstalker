/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include <qwidget.h>
#include <qstring.h>
#include <qcolor.h>
#include <qfont.h>
#include <qstringlist.h>
#include "Indicator.h"
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
    ~Plot ();
    void clear ();
    void setData (BarData *);
    void setMainFlag (bool);
    void setLogScale (bool);
    int setChartType (QString &);
    void setChartInput ();
    void setHideMainPlot (bool);
    bool getHideMainPlot ();
    void updateStatusBar (int, int);
    void setTabFlag (bool);
    bool getTabFlag ();
    bool getMainFlag ();
    bool getCrosshairsStatus ();
    void setInfoFlag (bool);
    void drawCrossHair ();
    void addIndicator (QString &, Indicator *);
    Indicator * getIndicator (QString &);
    void getIndicators (QStringList &);
    bool deleteIndicator (QString &);
    void addChartObject (Setting &);
    int getWidth ();
    int getPixelspace ();
    int getMinPixelspace ();
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setDrawMode (bool);
    void setDateFlag (bool);
    void setCrosshairsFlag (bool);
    IndicatorPlot * getIndicatorPlot ();
    DatePlot * getDatePlot ();

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void setInterval(BarData::BarCompression);
    void crossHair (int, int, bool);
    void setChartPath (QString);
    void setCrosshairsStatus (bool);
    void slotSliderChanged (int);
    void slotGridChanged (bool);
    void slotScaleToScreenChanged (bool);
    void slotDrawModeChanged (bool);
    void slotDateFlagChanged (bool);
    void slotLogScaleChanged (bool);
    void slotHideMainChanged (bool);
    void slotUpdateScalePlot ();

  private:
    bool dateFlag;
    bool tabFlag;
    DatePlot *datePlot;
    ScalePlot *scalePlot;
    IndicatorPlot *indicatorPlot;
};

#endif

