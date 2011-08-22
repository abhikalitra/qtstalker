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
#include <QMenu>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_grid.h>
//#include <QGestureEvent>
//#include <QTapAndHoldGesture>
//#include <QSwipeGesture>

#include "DateScaleDraw.h"
#include "PlotScaleDraw.h"
#include "Curve.h"
#include "PlotPicker.h"
#include "PlotMenu.h"
#include "ChartObject.h"
#include "CommandDialog.h"

class Plot : public QwtPlot
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalInfoMessage (Message);
    void signalIndex (int);

  public:
    Plot (QString, QWidget *);
    ~Plot ();
    void setDates ();
    void setCurve (Curve *);
    void setChartObjects (QHash<QString, ChartObject *>);
    void setHighLow ();
    void saveChartObjects ();
    void dates (QList<QDateTime> &);
    int index ();
    PlotMenu * plotMenu ();
    void setYPoints ();
    void addChartObject (ChartObject *);
    void unselect ();
    void select (QString);
    void showChartObjectMenu ();
    void setScriptFile (QString);
    QString scriptFile ();
    void setRow (int);
    int row ();
    void setCol (int);
    int col ();
    void setName (QString);
    QString name ();
    QHash<QString, Curve *> curves ();

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
    void mouseDoubleClick (int, QPoint);
    void deleteAllChartObjects ();
    void chartObjectNew (QString);
    void setCrossHairs (bool);
    void setCrossHairsColor (QColor);
    void setBarSpacing (int);
    void chartObjectDialog ();
    void deleteChartObject ();
    void deleteChartObject2 ();
    void chartObjectDialog2 ();
    void chartObjectDialog3 ();

  private:
    int _spacing;
    QHash<QString, ChartObject *> _chartObjects;
    QHash<QString, Curve *> _curves;
    DateScaleDraw *_dateScaleDraw;
    PlotScaleDraw *_plotScaleDraw;
    QwtPlotGrid *_grid;
    PlotPicker *_picker;
    double _high;
    double _low;
    int _startPos;
    int _endPos;
    ChartObject *_selected;
    PlotMenu *_menu;
    QMenu *_chartObjectMenu;
    bool _antiAlias;
    QString _name;
    CommandDialog *_chartObjectDialog;
    QString _scriptFile;
    int _row;
    int _col;
};

#endif
