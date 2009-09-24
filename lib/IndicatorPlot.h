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

#ifndef INDICATORPLOT_HPP
#define INDICATORPLOT_HPP

#include <QWidget>
#include <QString>
#include <QPixmap>
#include <QDateTime>
#include <QColor>
#include <QVector>
#include <QFont>
#include <QHash>
#include <QStringList>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QList>
#include <QRubberBand>
#include <QPoint>
#include <QRect>

#include "PlotLine.h"
#include "COSettings.h"
#include "Scaler.h"
#include "BarData.h"

#define HANDLE_WIDTH 6



class IndicatorPlot : public QWidget
{
  Q_OBJECT

  signals:
    void statusMessage (QString);
    void infoMessage (Setting *);
    void leftMouseButton (int, int, bool);
    void keyPressed (QKeyEvent *);
    void signalPixelspaceChanged (int, int);
    void signalDraw ();
    void signalDateFlag (bool);
    void signalLogFlag (bool);

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      COSelected,
      Moving,
      RubberBand,
      ClickWait2
    };

    IndicatorPlot (QWidget *);
    ~IndicatorPlot ();
    void clear ();
    void setData (BarData *);
    void setLogScale (bool);
    void updateStatusBar (int, int);
    void setInfoFlag (bool);
    void drawCrossHair ();
    int getWidth ();
    void strip (double, int, QString &);
    int convertXToDataIndex (int);
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setDrawMode (bool);
    void setDateFlag (bool);
    void setCrosshairsFlag (bool);
    void setScaler (Scaler &);
    Scaler & getScaler ();
    void getInfo (int x, int y);
    void getCOInfo ();
    void setXGrid (QVector<int> &);
    void setMenuFlag (bool);
    void drawLine ();
    void drawHorizontalLine ();
    void drawHistogram ();
    void drawHistogramBar ();
    void drawDot ();
    void drawBar ();
    void drawCandle ();
    void getPlotList (QList<PlotLine *> &);
    void setPlotList (QList<PlotLine *> &);
    void setIndicator (QString &);
    void loadChartObjects ();
    void addLine (PlotLine *);
    
    void objectClickWait ();
    void objectClickWait2 ();
    void drawBuyArrow ();
    void drawSellArrow ();
    void drawText ();
    void drawFiboLine ();
    void drawCOHorizontalLine ();
    void drawTrendLine ();
    void drawVerticalLine ();
    void objectMoving ();
    void trendLineMoving ();
    void fiboLineMoving ();

    void drawRubberBand (QRect &r);
   
  public slots:
    void draw();
    void drawRefresh();
    void setPixelspace (int);
    void setBackgroundColor (QColor &);
    void setBorderColor (QColor &);
    void setGridColor (QColor &);
    void setPlotFont (QFont &);
    void setIndex (int);
    void crossHair (int, int, bool);
    void showPopupMenu ();
    void setChartPath (QString &);
    void setCrosshairsStatus (bool);
    void slotSliderChanged (int);
    void slotGridChanged (bool);
    void slotScaleToScreenChanged (bool);
    void slotDrawModeChanged (bool);
    void slotLogScaleChanged (bool);
    void setInterval(BarData::BarLength);
    void slotNewBuyArrow ();
    void slotNewFiboline ();
    void slotNewHorizontalLine ();
    void slotNewSellArrow ();
    void slotNewText ();
    void slotNewTrendLine ();
    void slotNewVerticalLine ();
    void calculate ();

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void keyPressEvent (QKeyEvent *);
    virtual void mouseDoubleClickEvent (QMouseEvent *);
    virtual void contextMenuEvent (QContextMenuEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

  private slots:
    void drawObjects ();
    void drawLines ();
    void drawXGrid ();
    void drawYGrid ();
    void drawInfo ();
    void setScale ();
    int getXFromDate (QDateTime &);
    void getXY (int, int);
    void slotMessage (QString);
    void slotNewChartObject (int);
    void slotDeleteAllChartObjects ();
    void slotChartObjectDeleted ();
    void toggleDate ();
    void toggleLog ();
    void saveChartObjects ();
    void slotObjectDialog ();

  private:
    QFont plotFont;
    QFontMetrics *plotFontMetrics;
    PlotLine *currentLine;
    QPixmap buffer;
    int pixelspace;
    int startX;
    int startIndex;
    BarData::BarLength interval;
    QColor backgroundColor;
    QColor gridColor;
    QColor borderColor;
    bool gridFlag;
    bool scaleToScreen;
    bool logScale;
    bool crossHairFlag;
    bool drawMode;
    bool crosshairs;
    bool infoFlag;
    bool dateFlag;
    bool menuFlag;
    QDateTime crossHairX;
    double crossHairY;
    Scaler scaler;
    double y1;
    QDateTime x1;
    MouseStatus mouseFlag;
    QHash<QString, COSettings *> coList;
    COSettings *coSelected;
    COSettings *coDraw;
    QString indicator;
    BarData *data;
    QVector<int> xGrid;
    QMenu *chartMenu;
    QMenu *chartObjectMenu;
    QMenu *coMenu;
    QList<PlotLine *> plotList;
    QString chartSymbol;
    int moveFlag;
    QString dateFormat;

    int tx, ty;
    QRubberBand *rubberBand;
    QPoint mouseOrigin;
};

#endif


