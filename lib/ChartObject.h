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

#ifndef CHART_OBJECT_HPP
#define CHART_OBJECT_HPP

#include <QString>
#include <QRegion>
#include <QDateTime>
#include <QColor>
#include <QObject>
#include <QList>
#include <QPoint>
#include <QHash>
#include <QFont>

class ChartObject : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    enum Parm
    {
      ParmSave,
      ParmSelected,
      ParmSymbol,
      ParmID,
      ParmIndicator,
      ParmPlugin,
      ParmHigh,
      ParmLow,
      ParmColor,
      ParmDate,
      ParmDate2,
      ParmPrice,
      ParmPrice2,
      ParmFont,
      ParmLabel,
      ParmExtend,
      ParmLine1,
      ParmLine2,
      ParmLine3,
      ParmLine4,
      ParmLine5,
      ParmLine6
    };

    ChartObject ();
    ~ChartObject ();
    void setData (Parm, int);
    int getInt (Parm);
    void setData (Parm, double);
    double getDouble (Parm);
    void setData (Parm, QDateTime &);
    void getDate (Parm, QDateTime &);
    void setData (Parm, QString &);
    void getData (Parm, QString &);
    void setData (Parm, QColor &);
    void getColor (Parm, QColor &);
    void setData (Parm, QFont &);
    void getFont (Parm, QFont &);
    void message (QString &);
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    int isSelected (QPoint &);
    int isGrabSelected (QPoint &);

  protected:
    QHash<int, QString> parms;
    QList<QRegion *> grabHandles;
    QList<QRegion *> selectionArea;
};

#endif
