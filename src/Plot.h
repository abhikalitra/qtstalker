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
#include "BarData.h"
#include "ChartObject.h"
#include "Config.h"
#include "Plugin.h"

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
    void signalEditChartObject (ChartObject *, Plot *);
    void signalDeleteChartObject (QString, Plot *);
    void signalNewIndicator ();
    void signalNewChartObject (QString, Plot *);

  public:

    enum TimeInterval
    {
      Daily,
      Weekly,
      Monthly
    };

    enum MouseStatus
    {
      None,
      ClickWait,
      ClickWait2
    };

    Plot (QWidget *);
    ~Plot ();
    void clear ();
    void setData (BarData *);
    void setScaleWidth (int);
    void setDateHeight (int);
    void setMainFlag (bool);
    void setLogScale (bool);
    void setChartType (QString);
    void setChartInput ();
    void setConfig (Config *);
    void createChartObject (QString, QString);
//    void setPaintBars (QList<QColor>);
    void setHideMainPlot (bool);
    bool getHideMainPlot ();
    void updateStatusBar (int, int);
    void setTabFlag (bool);
    bool getTabFlag ();
    bool getMainFlag ();

    void addIndicator (QString, Indicator *);
    Indicator * getIndicator (QString);
    QStringList getIndicators ();
    void deleteIndicator (QString);
    QStringList getChartObjectList ();
    void newChartObject ();
    void addChartObject (ChartObject *);
    void addChartObject (Setting *);
    QStringList getChartObjects ();

    int getWidth ();
    int getPixelspace ();
    int getMinPixelspace ();
    QString strip (double);

  public slots:
    void draw();
    void setGridFlag (bool);
    void setScaleToScreen (bool);
    void setPixelspace (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setPlotFont (QFont);
    void setIndex (int);
    void setInterval(Plot::TimeInterval);
    void setDateFlag (bool);
    void crossHair (int, int);
    void printChart ();
    void showPopupMenu ();

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void mousePressEvent (QMouseEvent *);
    virtual void mouseMoveEvent (QMouseEvent *);
    virtual void keyPressEvent (QKeyEvent *);

  private slots:
    void drawObjects ();
    void drawLines ();
    void drawDate ();
    void drawXGrid ();
    void drawScale ();
    void drawYGrid ();
    void drawInfo ();
    void setHeight ();
    void setWidth ();
    void setScale ();
    int getXFromDate (QDateTime);
    void getXY (int, int, int);
    void createXGrid ();

    void slotEditIndicator (int);
    void slotDeleteIndicator (int);
    void slotEditChartObject (int);
    void slotDeleteChartObject (int);
    void slotNewIndicator ();
    void slotNewChartObject (int);
    
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
    TimeInterval interval;
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
    int crossHairX;
    int crossHairY;

    int scaleWidth;
    double mainHigh;
    double mainLow;
    QArray<double> scaleArray;
    Scaler *scaler;

    QString y1;
    QString y2;
    QString x1;
    QString x2;
    QString objectName;
    MouseStatus mouseFlag;
    ChartObject::ObjectType objectFlag;

    QString chartType;
    BarData *data;
    QDict<Indicator> indicators;
    QDict<ChartObject> chartObjects;
//    QList<QColor> paintBars;
    QArray<int> xGrid;

    QPopupMenu *chartMenu;
    QPopupMenu *chartEditMenu;
    QPopupMenu *chartDeleteMenu;
    QPopupMenu *chartObjectDeleteMenu;
    QPopupMenu *chartObjectEditMenu;
    QPopupMenu *chartObjectMenu;
};

#endif


