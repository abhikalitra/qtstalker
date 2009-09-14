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

#ifndef COSETTINGS_HPP
#define COSETTINGS_HPP

#include <QString>
#include <QHash>
#include <QColor>
#include <QDateTime>
#include <QRegion>
#include <QList>
#include <QFont>
#include <QPoint>



class COSettings
{
  public:
    enum COType
    {
      COBuyArrow,
      COUnused,
      COFiboLine,
      COHorizontalLine,
      COSellArrow,
      COText,
      COTrendLine,
      COVerticalLine
    };

    enum COParm
    {
      COID,
      COSymbol,
      COIndicator,
      COCOType,
      CODate,
      CODate2,
      COValue,
      COValue2,
      COColor,
      COFont,
      COHigh,
      COLow,
      COExtend,
      COLine1,
      COLine2,
      COLine3,
      COLine4,
      COLine5,
      COLine6,
      COUseBar,
      COBarField,
      COLabel
    };

    COSettings (QString i, QString s, QString indi, QString t);
    COSettings ();
    ~COSettings ();
    void getSettings (QString &);
    void parse (QString &);

    void getData (COParm, int &);
    void setData (COParm, int);
    void getData (COParm, double &);
    void setData (COParm, double);
    void getData (COParm, QColor &);
    void setData (COParm, QColor &);
    void getData (COParm, QFont &);
    void setData (COParm, QFont &);
    void getData (COParm, QDateTime &);
    void setData (COParm, QDateTime &);
    void getData (COParm, QString &);
    void setData (COParm, QString &);
    
    void newObject (int);
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
    void getHighLow (double &high, double &low);
    
  protected:
    QString id;
    QString symbol;
    QString indicator;
    QString type;
    QFont font;
    QHash<COParm, QString> settings;
    QString dateFormat;
    int saveFlag;
    int selected;
    QList<QRegion *> grabHandles;
    QList<QRegion *> selectionArea;
};

#endif
