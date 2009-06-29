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

#ifndef COBASE_HPP
#define COBASE_HPP

#include "Scaler.h"
#include "BarData.h"
#include "COSettings.h"
#include <QPixmap>
#include <QObject>
#include <QString>
#include <QMenu>
#include <QPoint>
#include <QRegion>
#include <QColor>
#include <QList>
#include <QDateTime>
#include <QKeyEvent>


#define HANDLE_WIDTH 6

class COBase : public QObject
{
  Q_OBJECT
  
  signals:
    void signalDraw ();
    void signalRefresh ();
    void message (QString);
    void signalObjectDeleted (QString);

  public:
    enum Status
    {
      None,
      ClickWait,
      ClickWait2,
      Selected,
      Moving
    };

    enum COType
    {
      COBuyArrow,
      COCycle,
      COFiboLine,
      COHorizontalLine,
      COSellArrow,
      COText,
      COTrendLine,
      COVerticalLine
    };

    COBase ();
    virtual ~COBase ();
    virtual void draw (QPixmap &, Scaler &, int, int, int);
    virtual void newObject (QString &, QString &);
    virtual void keyEvent (QKeyEvent *);
    virtual double getHigh ();
    virtual double getLow ();
    virtual void loadSettings (COSettings &);
    virtual void saveSettings ();
    virtual void adjustForSplit (QDateTime &, double);
    
    void setData (BarData *);
    QString getHelpFile ();
    void setSaveFlag (bool);
    bool getSaveFlag ();
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    QString getID ();
    void setStatus (Status);
    COBase::Status getStatus ();
    bool isSelected (QPoint point);
    bool isGrabSelected (QPoint point);
    COBase * getCO (int);
    void setSymbol (QString &);

  public slots:    
    virtual void prefDialog ();
    virtual COBase::Status pointerClick (QPoint &, QDateTime &, double);
    virtual void pointerMoving (QPixmap &, QPoint &, QDateTime &, double y);

    void removeObject ();
    void moveObject ();
    void showMenu ();
    
  protected:
    BarData *data;
    QMenu *menu;
    QString indicator;
    QString symbol;
    QString id;
    QString helpFile;
    int type;
    QString dateFormat;
    Status status;
    bool saveFlag;
    QList<QRegion *> grabHandles;
    QList<QRegion *> selectionArea;
    QColor color;
    QDateTime date;
    double value;
};

#endif
