/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001,2002 Stefan S. Stratigakos
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
#include "PlotLine.h"
#include "Setting.h"
#include "Indicator.h"

#define SCALE_WIDTH 60
#define DATE_HEIGHT 20
#define INFO_TEXT_OFFSET 11

class Plot : public QWidget
{
  Q_OBJECT

  signals:
    void rightMouseButton ();
    void statusMessage (QString);

  public:

    enum TimeInterval
    {
      Daily,
      Weekly,
      Monthly
    };

    Plot (QWidget *);
    ~Plot ();
    void clear ();
    void setData (QList<Setting>);
    void setInterval(TimeInterval);
    void setScaleWidth (int);
    void setDateHeight (int);
    void setBackgroundColor (QColor);
    void setBorderColor (QColor);
    void setGridColor (QColor);
    void setUpColor (QColor);
    void setDownColor (QColor);
    void setNeutralColor (QColor);
    void setPlotFont (QFont);
    void setDateFlag (bool);
    void setGridFlag (bool);
    void setMainFlag (bool);
    void setScaleToScreen (bool);
    void setOtherFlag (bool);
    bool getOtherFlag ();
    void setPixelspace (int);
    void setChartType (QString);
    void setAlerts (QMemArray<int>);

    void addIndicator (QString, Indicator *);
    Indicator * getIndicator (QString);
    QStringList getIndicators ();
    void deleteIndicator (QString);
    void setCurrentIndicator (QString);
    QStringList getChartObjectList ();
    Setting * newChartObject (QString);

    void crossHair (int, int);
    void setIndex (int);
    int getWidth ();
    int getPixelspace ();
    int getMinPixelspace ();
    int getDataSize ();
    QDateTime getDate (int);
    double getOpen (int);
    double getHigh (int);
    double getLow (int);
    double getClose (int);
    QString strip (double);

  public slots:
    void draw();

  protected:
    virtual void paintEvent (QPaintEvent *);
    virtual void resizeEvent (QResizeEvent *);
    virtual void mousePressEvent (QMouseEvent *);

  private slots:
    void drawVerticalLine (Setting *);
    void drawTrendLine (Setting *);
    void drawText (Setting *);
    void drawSellArrow (Setting *);
    void drawHorizontalLine (Setting *);
    void drawFibonacciLine (Setting *);
    void drawFibonacciLine2 (QColor, QString, double, double, double);
    void drawBuyArrow (Setting *);
    void drawPointAndFigure ();
    void drawPaintBar ();
    void drawLineChart ();
    void drawCandle ();
    void drawBars ();
    void drawObjects ();
    void drawLines ();
    void drawDate ();
    void drawXGrid ();
    void drawScale ();
    void drawYGrid ();
    void drawLine ();
    void drawHorizontalLine ();
    void drawHistogram ();
    void drawHistogramBar ();
    void drawDot ();
    void drawInfo ();
    void setHeight ();
    void setWidth ();
    double convertToVal (int);
    int convertToY (double);
    void setScale ();
    int getXFromDate (QDateTime);

  private:
    QFont plotFont;
    QMemArray<int> alerts;
    PlotLine *currentLine;
    QPixmap buffer;
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
    QColor upColor;
    QColor downColor;
    QColor neutralColor;
    bool dateFlag;
    bool gridFlag;
    bool mainFlag;
    bool scaleToScreen;

    int scaleWidth;
    double scaleHigh;
    double scaleLow;
    double scaler;
    double mainHigh;
    double mainLow;
    QArray<double> scaleArray;

    QString chartType;
    QString currentIndicator;
    QList<Setting> data;
    QDict<Indicator> indicators;
};

#endif


