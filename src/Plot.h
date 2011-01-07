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
#include <QColor>
#include <QFont>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
#include <QMainWindow>

#include "DateScaleDraw.h"
#include "PlotScaleDraw.h"
#include "Curve.h"
#include "PlotPicker.h"
#include "Setting.h"
#include "ChartObject.h"
#include "PlotMenu.h"
#include "Indicator.h"
#include "DockWidget.h"

class Plot : public QwtPlot
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalInfoMessage (Setting);
    void signalClick (int, QPoint);
    void signalMove (QPoint);
    void signalIndex (int);

  public:
    Plot (QString, QMainWindow *);
    ~Plot ();
    void setDates ();
    void addCurve (QString id, Curve *);
    void addCurves (QHash<QString, Curve *> &);
    void addChartObjects (QHash<QString, Setting> &);
    void setIndicator ();
    Indicator * indicator ();
    void addCurve2 (Curve *curve, QwtPlotCurve *qcurve);
    void addCurve3 (QString id, Curve *curve, QwtPlotCurve *qcurve);
    void setHighLow ();
    void setupChartObject (ChartObject *);
    void saveChartObjects ();
    void dates (QList<QDateTime> &);
    int index ();
    PlotMenu * plotMenu ();
    DockWidget * dockWidget ();

  public slots:
    virtual void clear ();
    void updatePlot ();
    void loadSettings ();
    void setStartIndex (int index);
    void setBackgroundColor (QColor);
    void setFont (QFont);
    void setGridColor (QColor);
    void setGrid (bool);
    void showDate (bool);
    void setLogScaling (bool);
    void showContextMenu ();
    void mouseMove (QPoint);
    void mouseClick (int, QPoint);
    void deleteChartObject (QStringList);
    void updateChartObject (QString);
    void chartObjectNew (QString, QString);
    void chartObjectSelected (QString);
    void chartObjectUnselected (QString);
    void chartObjectMoveStart (QString);
    void chartObjectMoveEnd (QString);
    void setCrossHairs (bool);
    void setCrossHairsColor (QColor);
    void setBarSpacing (int);

  private:
    int _spacing;
    QHash<QString, QwtPlotCurve *> _qwtCurves;
    QHash<QString, ChartObject *> _chartObjects;
    DateScaleDraw *_dateScaleDraw;
    PlotScaleDraw *_plotScaleDraw;
    QwtPlotGrid *_grid;
    PlotPicker *_picker;
    Indicator *_indicator;
    double _high;
    double _low;
    int _startPos;
    int _endPos;
    int _selected;
    PlotMenu *_menu;
    DockWidget *_dock;
};

#endif
