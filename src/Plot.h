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
#include "ChartObject.h"
#include "Config.h"
#include "Plugin.h"
#include "ChartDb.h"

class Plot : public QWidget
{
  Q_OBJECT

  signals:
    void statusMessage (QString);
    void chartObjectCreated (ChartObject *);
    void infoMessage (Setting *);
    void leftMouseButton (int, int, bool);
    void keyPressed (QKeyEvent *);
    void signalEditIndicator (QString, Plot *);
    void signalDeleteIndicator (QString, Plot *);
    void signalNewIndicator ();
    void signalMinPixelspace (int);
    void signalCrosshairsStatus (bool);

  public:
    enum MouseStatus
    {
      None,
      ClickWait,
      ClickWait2,
      COSelected,
      Moving
    };

    Plot (QWidget *);
    ~Plot ();
    void clear ();
    void setData (BarData *);
    void setScaleWidth (int);
    void setDateHeight (int);
    void setMainFlag (bool);
    void setLogScale (bool);
    int setChartType (QString);
    void setChartInput ();
    void setConfig (Config *);
    void createChartObject (QString, QString);
    void setHideMainPlot (bool);
    bool getHideMainPlot ();
    void updateStatusBar (int, int);
    void setTabFlag (bool);
    bool getTabFlag ();
    bool getMainFlag ();
    bool getCrosshairsStatus ();

    void addIndicator (QString, Indicator *);
    Indicator * getIndicator (QString);
    QStringList getIndicators ();
    void deleteIndicator (QString);
    QStringList getChartObjectList ();
    void newChartObject ();
    void addChartObject (ChartObject *);
    void addChartObject (Setting *);
    QStringList getChartObjects ();
    void isChartObjectSelected (int x, int y);

    int getWidth ();
    int getPixelspace ();
    int getMinPixelspace ();
    QString strip (double);

  public slots:
    void draw();
    void drawRefresh();
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void setInterval(ChartDb::BarCompression);
    void setDateFlag (bool);
    void crossHair (int, int);
    void printChart ();
    void showPopupMenu ();
    void setChartPath (QString);
    void setDrawMode (bool);
    void slotSaveChartObjects ();
    void setCrosshairsStatus (bool);

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
    void drawScale ();
    void drawYGrid ();
    void drawInfo ();
    void setHeight ();
    void setWidth ();
    void setScale ();
    int getXFromDate (BarDate);
    void getXY (int, int, int);
    void createXGrid ();
    void slotMessage (QString);
    void objectMoving ();
    void toggleCrosshairs ();
    
    void drawDate ();
    void drawDailyDate ();
    void drawWeeklyDate ();
    void drawMonthlyDate ();
    void drawHourlyDate ();
    void draw15Date ();

    void slotEditIndicator (int);
    void slotDeleteIndicator (int);
    void slotDeleteChartObject (QString);
    void slotNewIndicator ();
    void slotNewChartObject (int);
    void slotChartObjectSelected (ChartObject *);
    void slotDeleteAllChartObjects ();
    
    void slotEditChartPrefs ();

  private:
    QFont plotFont;
    PlotLine *currentLine;
    QPixmap *buffer;
    Plugin *chartPlugin;
    Config *config;
    int pixelspace;
    int minPixelspace;
    int dateHeight;
    int _height;
    int _width;
    int startX;
    int startIndex;
    ChartDb::BarCompression interval;
    QColor backgroundColor;
    QColor gridColor;
    QColor borderColor;
    bool dateFlag;
    bool gridFlag;
    bool mainFlag;
    bool scaleToScreen;
    bool logScale;
    bool hideMainPlot;
    bool tabFlag;
    bool crossHairFlag;
    bool drawMode;
    bool crosshairs;
    int crossHairX;
    int crossHairY;
    int tx, tx2; // temp x coords for drawing chart objects
    int ty, ty2;  // temp y coords for drawing chart objects

    int scaleWidth;
    double mainHigh;
    double mainLow;
    QMemArray<double> scaleArray;
    Scaler *scaler;

    double y1;
    double y2;
    BarDate x1;
    BarDate x2;
    QString objectName;
    MouseStatus mouseFlag;
    ChartObject::ObjectType objectFlag;
    ChartObject *tco;

    QString chartType;
    QString chartPath;
    BarData *data;
    QDict<Indicator> indicators;
    QDict<ChartObject> chartObjects;
    QMemArray<int> xGrid;

    QPopupMenu *chartMenu;
    QPopupMenu *chartEditMenu;
    QPopupMenu *chartDeleteMenu;
    QPopupMenu *chartObjectMenu;
};

#endif


