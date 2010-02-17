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

#ifndef BASECO_HPP
#define BASECO_HPP

#include <QString>
#include <QRegion>
#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QDateTime>
#include <QColor>
#include <QObject>
#include <QSqlQuery>

#include "BarData.h"
#include "Scaler.h"
#include "Setting.h"

class BaseCO : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum COType
    {
      COBuyArrow,
      COHorizontalLine,
      CORetracement,
      COSellArrow,
      COText,
      COTrendLine,
      COVerticalLine
    };

    BaseCO ();
    virtual ~BaseCO ();
    virtual void draw (QPixmap &, BarData *data, int startX, int pixelspace, int startIndex,
		       Scaler &scaler);
    virtual void getHighLow (double &h, double &l);
    virtual void getInfo (Setting &);
    virtual void dialog ();
    virtual void setDate (QDateTime &);
    virtual void getDate (QDateTime &);
    virtual void setDate2 (QDateTime &);
    virtual void getDate2 (QDateTime &);
    virtual void setPrice (double);
    virtual void setPrice2 (double);
    virtual void setSettings (QSqlQuery &q);
    virtual void getSettings (QString &);
    virtual void create (QString &_id, QString &_symbol, QString &_indicator);
    virtual int create2 (QDateTime &x, double y);
    virtual int create3 (QDateTime &x, double y);
    virtual void moving (QDateTime &x, double y, int moveFlag);

    void setSaveFlag (int);
    int getSaveFlag ();
    void setSelected (int);
    int getSelected ();
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    int isSelected (QPoint &point);
    int isGrabSelected (QPoint &point);
    void getCOList (QStringList &);
    void setSymbol (QString &);
    void getSymbol (QString &);
    void setID (QString &);
    void getID (QString &);
    void setIndicator (QString &);
    void getIndicator (QString &);
    int getType ();
    void setHigh (double);
    double getHigh ();
    void setLow (double);
    double getLow ();

  protected:
    QColor color;
    QString id;
    QString symbol;
    QString indicator;
    int type;
    int saveFlag;
    int selected;
    int handleWidth;
    double high;
    double low;
    QList<QRegion *> grabHandles;
    QList<QRegion *> selectionArea;
};

#endif
