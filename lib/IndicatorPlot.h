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

#ifndef INDICATORPLOT_HPP
#define INDICATORPLOT_HPP

#include <qwidget.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qdatetime.h>
#include <qcolor.h>
#include <qmemarray.h>
#include <qfont.h>
#include <qdict.h>
#include <qstringlist.h>
#include <qpopupmenu.h>

#include "PlotLine.h"
#include "Setting.h"
#include "Indicator.h"
#include "Scaler.h"
#include "Config.h"
#include "ChartPlugin.h"
#include "BarData.h"
#include "COPlugin.h"

class IndicatorPlot : public QWidget
{
  Q_OBJECT

  signals:
    void statusMessage (QString);
    void infoMessage (Setting *);
    void leftMouseButton (int, int, bool);
    void keyPressed (QKeyEvent *);
    void signalEditIndicator (QString);
    void signalNewIndicator ();
    void signalMinPixelspace (int);
    void signalCrosshairsStatus (bool);
    void signalDraw ();

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      COSelected,
      Moving
    };

    IndicatorPlot (QWidget *);
    ~IndicatorPlot ();
    void clear ();
    void setData (BarData *);
    void setMainFlag (bool);
    void setLogScale (bool);
    int setChartType (QString &);
    void setChartInput ();
    void setHideMainPlot (bool);
    bool getHideMainPlot ();
    void updateStatusBar (int, int);
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
    void strip (double, int, QString &);
    int convertXToDataIndex (int);
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setDrawMode (bool);
    void setDateFlag (bool);
    void setCrosshairsFlag (bool);
    void setScaler (Scaler &);
    Scaler & getScaler ();
    void getInfo (int);
   
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
    void printChart ();
    void showPopupMenu ();
    void setChartPath (QString &);
    void setCrosshairsStatus (bool);
    void slotSliderChanged (int);
    void slotGridChanged (bool);
    void slotScaleToScreenChanged (bool);
    void slotDrawModeChanged (bool);
    void slotLogScaleChanged (bool);
    void slotHideMainChanged (bool);
    void setInterval(BarData::BarCompression);

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
    void drawXGrid ();
    void drawYGrid ();
    void drawInfo ();
    void setScale ();
    int getXFromDate (BarDate &);
    void getXY (int, int);
    void createXGrid ();
    void slotMessage (QString);
    void toggleCrosshairs ();
    void slotEditIndicator (int);
    void slotNewIndicator ();
    void slotNewChartObject (int);
    void slotDeleteAllChartObjects ();
    void slotEditChartPrefs ();

  private:
    QFont plotFont;
    PlotLine *currentLine;
    QPixmap buffer;
    ChartPlugin *chartPlugin;
    Config config;
    int pixelspace;
    int minPixelspace;
    int startX;
    int startIndex;
    BarData::BarCompression interval;
    QColor backgroundColor;
    QColor gridColor;
    QColor borderColor;
    bool gridFlag;
    bool mainFlag;
    bool scaleToScreen;
    bool logScale;
    bool hideMainPlot;
    bool crossHairFlag;
    bool drawMode;
    bool crosshairs;
    bool infoFlag;
    BarDate crossHairX;
    double crossHairY;
    Scaler scaler;
    double y1;
    BarDate x1;
    MouseStatus mouseFlag;
    COPlugin *coPlugin;
    QStringList coList;
    QDict<COPlugin> coPlugins;
    QString chartType;
    QString chartPath;
    BarData *data;
    QDict<Indicator> indicators;
    QMemArray<int> xGrid;
    QPopupMenu *chartMenu;
    QPopupMenu *chartEditMenu;
    QPopupMenu *chartObjectMenu;
};

#endif


