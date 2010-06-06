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

#include <QWidget>
#include <QString>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QPixmap>
#include <QMenu>
#include <QPoint>
#include <QList>

#include "COPlugin.h"
#include "Setting.h"
#include "BarData.h"
#include "PlotGrid.h"
#include "PlotData.h"
#include "Indicator.h"
#include "DatePlot.h"
#include "ScalePlot.h"
#include "Scaler.h"
#include "DateBar.h"
#include "PlotCursor.h"

class Plot : public QWidget
{
  Q_OBJECT

  signals:
    void signalStatusMessage (QString);
    void signalInfoMessage (Setting *);
    void signalPixelSpaceChanged (int, int);
    void signalDraw ();
    void signalDateFlag (bool);
    void signalLogFlag (bool);
    void signalNewExternalChartObjectDone();
    void signalIndexChanged (int);

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      COSelected,
      Moving,
      ClickWait2,
      NewObjectWait
    };

    Plot (QWidget *);
    ~Plot ();
    void setData (BarData *);
    void setLogScale (bool);
    bool logScale ();
    void setInfoFlag (bool);
    int convertXToDataIndex (int);
    void setGridFlag (bool);
    void setDateFlag (bool);
    void setXGrid (QList<int> &);
    void setMenuFlag (bool);
    void setIndicator (Indicator &);
    Indicator & indicator ();
    void loadChartObjects ();
    void setScale ();
    DateBar & dateBars ();
    int width ();

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void showPopupMenu ();
    void sliderChanged (int);
    void gridChanged (bool);
    void logScaleChanged (bool);
    void setInterval(Bar::BarLength);
    void newExternalChartObject (QString);
    void setExternalChartObjectFlag ();
    void cursorChanged (int);
    void clear ();

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void mouseDoubleClickEvent (QMouseEvent *);
    virtual void contextMenuEvent (QContextMenuEvent *);

  private slots:
    void drawObjects ();
    void drawLines ();
    void getXY (int, int);
    void newChartObject (QString);
    void deleteAllChartObjects ();
    void chartObjectDeleted ();
    void toggleDate ();
    void toggleLog ();
    void saveChartObjects ();
    void objectDialog ();
    void coSelected (int);

  private:
    Indicator _indicator;
    PlotData _plotData;
    PlotGrid _grid;
    bool _menuFlag;
    COPlugin *_coSelected;
    QMenu *_chartMenu;
    QMenu *_coMenu;
    QString _symbol;
    QString _exchange;
    int _moveFlag;
    int _newObjectFlag;
    int _dragPos;
    MouseStatus _saveMouseFlag;
    DatePlot _datePlot;
    ScalePlot _scalePlot;
    Scaler _scaler;
    DateBar _dateBars;
    PlotCursor *_cursor;
};

#endif


