/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
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

#ifndef CO_PLUGIN_HPP
#define CO_PLUGIN_HPP

#include <QString>
#include <QIcon>
#include <QList>
#include <QRegion>
#include <QDateTime>
#include <QPoint>
#include <QObject>
#include <QtSql>
#include <QPixmap>

#include "Setting.h"
#include "PlotData.h"

class COPlugin : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (QString);

  public:
    COPlugin ();
    virtual ~COPlugin ();
    virtual void draw (PlotData &);
    virtual int getHighLow (double &, double &);
    virtual void getInfo (Setting *);
    virtual void dialog ();
    virtual void load (QSqlQuery &);
    virtual void save ();
    virtual void create ();
    virtual int create2 (QDateTime &, double);
    virtual int create3 (QDateTime &, double);
    virtual void moving (QDateTime &, double, int moveFlag);
    virtual void getIcon (QIcon &);
    virtual int inDateRange (QDateTime &, QDateTime &);

    void setSelected (int);
    void clearGrabHandles ();
    void setGrabHandle (QRegion *);
    void clearSelectionArea ();
    void setSelectionArea (QRegion *);
    int isSelected (QPoint &);
    int isGrabSelected (QPoint &);
    int getID ();
    void setID (int);
    void setSymbol (QString &);
    void setIndicator (QString &);

  protected:
    int handleWidth;
    int selected;
    int saveFlag;
    int id;
    QString symbol;
    QString indicator;
    QString plugin;
    QList<QRegion *> grabHandles;
    QList<QRegion *> selectionArea;
};

#endif
