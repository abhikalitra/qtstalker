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

#ifndef PLOT_HPP
#define PLOT_HPP

#include <QString>
#include <QHash>
#include <QDateTime>
#include <QMap>
#include <QMenu>
#include <QColor>
#include <QFont>
#include <QAction>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_engine.h>

#include "BarData.h"
#include "DateScaleDraw.h"
#include "PlotScaleDraw.h"
#include "Curve.h"
#include "PlotPicker.h"
#include "Setting.h"
#include "ChartObject.h"
#include "ChartObjectDialog.h"
#include "ChartObjectSettings.h"

class Plot : public QwtPlot
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalNewIndicator ();
    void signalEditIndicator (QString);
    void signalDeleteIndicator (QString);
    void signalBackgroundColorChanged (QColor);
    void signalFontChanged (QFont);
    void signalInfoMessage (Setting);

  public:
    Plot ();
    ~Plot ();
    void updatePlot ();
    void setDates (BarData &);
    void addCurve (int id, Curve *);
    void addCurves (QMap<int, Curve *> &);
    void setIndicator (QString &);
    void setHighLow ();
    void addCurve2 (Curve *curve, QwtPlotCurve *qcurve);
    void addCurve3 (int id, Curve *curve, QwtPlotCurve *qcurve);
    void loadChartObjects ();

  public slots:
    virtual void clear ();
    void setStartIndex (int index);
    void setBackgroundColor (QColor);
    void setFont (QFont);
    void setGridColor (QColor);
    void setGrid (bool);
    void showDate (bool);
    void setLogScaling (bool);
    void setCrosshair (bool);
    void showContextMenu ();
    void editIndicator ();
    void deleteIndicator ();
    void deleteAllChartObjects ();
    void toggleDate ();
    void toggleLog ();
    void editBackgroundColor ();
    void editFont ();
    void mouseMove (QPoint);
    void mouseClick (int, QPoint);
    void newChartObject (ChartObjectSettings);
    void updateChartObject (ChartObjectSettings);
    void deleteChartObject (ChartObjectSettings);
    void chartObjectMenuSelected (QAction *);

private:
    int _spacing;
    QMap<int, QwtPlotCurve *> _qwtCurves;
    QMap<int, Curve *> _curves;
    QMap<int, ChartObject *> _chartObjects;
    DateScaleDraw *_dateScaleDraw;
    PlotScaleDraw *_plotScaleDraw;
    QwtPlotGrid *_grid;
    QwtLinearScaleEngine *_linearScaleEngine;
    QwtLog10ScaleEngine *_logScaleEngine;
    QMenu *_chartMenu;
    QMenu *_coListMenu;
    PlotPicker *_picker;
    QString _indicator;
    QString _exchange;
    QString _symbol;
    double _high;
    double _low;
};

#endif


