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
#include "Config.h"
#include "ChartPlugin.h"
#include "BarData.h"
#include "COPlugin.h"

class Plot : public QWidget
{
  Q_OBJECT

  signals:
    void statusMessage (QString);
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
    void setHideMainPlot (bool);
    bool getHideMainPlot ();
    void updateStatusBar (int, int);
    void setTabFlag (bool);
    bool getTabFlag ();
    bool getMainFlag ();
    bool getCrosshairsStatus ();
    void setInfoFlag (bool);
    void drawCrossHair ();

    void addIndicator (QString, Indicator *);
    Indicator * getIndicator (QString);
    QStringList getIndicators ();
    void deleteIndicator (QString);
    void addChartObject (Setting *);

    int getWidth ();
    int getPixelspace ();
    int getMinPixelspace ();
    QString strip (double, int);
    int convertXToDataIndex (int);

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
    void setInterval(BarData::BarCompression);
    void setDateFlag (bool);
    void crossHair (int, int, bool);
    void printChart ();
    void showPopupMenu ();
    void setChartPath (QString);
    void setDrawMode (bool);
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
    void getXY (int, int);
    void createXGrid ();
    void slotMessage (QString);
    void toggleCrosshairs ();
    
    void drawDate ();
    void drawDailyDate ();
    void drawWeeklyDate ();
    void drawMonthlyDate ();
    void drawHourlyDate ();
    void draw15Date ();

    void slotEditIndicator (int);
    void slotDeleteIndicator (int);
    void slotNewIndicator ();
    void slotNewChartObject (int);
    void slotDeleteAllChartObjects ();
    
    void slotEditChartPrefs ();

  private:
    QFont plotFont;
    PlotLine *currentLine;
    QPixmap *buffer;
    ChartPlugin *chartPlugin;
    Config config;
    int pixelspace;
    int minPixelspace;
    int dateHeight;
    int _height;
    int _width;
    int startX;
    int startIndex;
    BarData::BarCompression interval;
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
    bool infoFlag;
    BarDate crossHairX;
    double crossHairY;

    int scaleWidth;
    double mainHigh;
    double mainLow;
    QMemArray<double> scaleArray;
    Scaler *scaler;

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
    QPopupMenu *chartDeleteMenu;
    QPopupMenu *chartObjectMenu;
};

#endif


