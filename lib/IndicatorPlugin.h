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

#ifndef INDICATORPLUGIN_HPP
#define INDICATORPLUGIN_HPP

#include "PlotLine.h"
#include "BarData.h"
#include "ta_libc.h"
#include "Indicator.h"
#include <QString>
#include <QStringList>
#include <QHash>
#include <QObject>
#include <QList>

class IndicatorPlugin : public QObject
{
  Q_OBJECT

  signals:
    void signalWakeup ();
  
  public:
    IndicatorPlugin(QString &, BarData *);
    IndicatorPlugin();
    ~IndicatorPlugin();
    void setDefaults ();
    void setIndicatorInput (BarData *);
    void getMATypes (QStringList &);
    void calculate (QList<PlotLine *> &);
    void createPlot (Indicator &, QHash<QString, PlotLine *> &, QList<PlotLine *> &);
    void setName (QString &);
    void getName (QString &);
    void getIndicatorList (QStringList &);
//    PlotLine * getMA (PlotLine *d, int, int);
    void printError (QString, TA_RetCode);
    void addInputLine (QString &, QHash<QString, PlotLine *> &);
    void getBARS (IndicatorParms &, QHash<QString, PlotLine *> &);
    void getUTIL (IndicatorParms &, QHash<QString, PlotLine *> &);

  public slots:
    void wakeup ();
    
  protected:
    BarData *data;
    QStringList lineTypes;
    QStringList maList;
    QString name;
};

#endif

