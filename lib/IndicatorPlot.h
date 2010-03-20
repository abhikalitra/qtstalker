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

#ifndef INDICATOR_PLOT_HPP
#define INDICATOR_PLOT_HPP

#include <QWidget>
#include <QString>
#include <QKeyEvent>
#include <QVector>
#include <QRect>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QDateTime>
#include <QPixmap>
#include <QMenu>
#include <QRubberBand>
#include <QPoint>
#include <QList>

#include "COPlugin.h"
#include "Setting.h"
#include "BarData.h"
#include "PlotGrid.h"
#include "PlotData.h"
#include "Indicator.h"

class IndicatorPlot : public QWidget
{
  Q_OBJECT

  signals:
    void signalStatusMessage (QString);
    void infoMessage (Setting *);
    void keyPressed (QKeyEvent *);
    void signalPixelspaceChanged (int, int);
    void signalDraw ();
    void signalDateFlag (bool);
    void signalLogFlag (bool);
    void signalNewExternalChartObjectDone();

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      COSelected,
      Moving,
      ClickWait2,
      NewObjectWait,
      CursorZoom,
      CursorCrosshair
    };

    IndicatorPlot (QWidget *);
    ~IndicatorPlot ();
    void clear ();
    void setData (BarData *);
    void setLogScale (bool);
    void setInfoFlag (bool);
    int getWidth ();
    int convertXToDataIndex (int);
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setDateFlag (bool);
    void setScaler (Scaler &);
    Scaler & getScaler ();
    void setXGrid (QVector<int> &);
    void setMenuFlag (bool);
    void setIndicator (Indicator &);
    void getIndicator (Indicator &);
    void loadChartObjects ();
    void getDateBar (DateBar &);
    void getScalePoints (QList<Setting> &);
    void setScale ();

  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor &);
    void setBorderColor (QColor &);
    void setGridColor (QColor &);
    void setPlotFont (QFont &);
    void setIndex (int);
    void showPopupMenu ();
    void setChartPath (QString &);
    void slotSliderChanged (int);
    void slotGridChanged (bool);
    void slotScaleToScreenChanged (bool);
    void slotLogScaleChanged (bool);
    void setInterval(BarData::BarLength);
    void newExternalChartObject (QString);
    void setExternalChartObjectFlag ();
    void cursorChanged (int);

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void keyPressEvent (QKeyEvent *);
    virtual void mouseDoubleClickEvent (QMouseEvent *);
    virtual void contextMenuEvent (QContextMenuEvent *);

  private slots:
    void drawObjects ();
    void drawLines ();
    void getXY (int, int);
    void slotNewChartObject (QString);
    void slotDeleteAllChartObjects ();
    void slotChartObjectDeleted ();
    void toggleDate ();
    void toggleLog ();
    void saveChartObjects ();
    void slotObjectDialog ();

  private:
    Indicator indicator;
    PlotData plotData;
    PlotGrid grid;
    BarData::BarLength interval;
    bool menuFlag;
    MouseStatus mouseFlag;
    COPlugin *coSelected;
    QMenu *chartMenu;
    QMenu *coMenu;
    QString chartSymbol;
    int moveFlag;
    QRubberBand *rubberBand;
    QPoint mouseOrigin;
    QString newChartObject;
};

#endif


