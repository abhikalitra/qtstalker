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

#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <QString>
#include <QList>
#include <QHash>

#include "PlotLine.h"
#include "Setting.h"
#include "COPlugin.h"

class Indicator
{
  public:
    Indicator ();
    void setName(QString &);
    void getName(QString &);
    void setEnable(int);
    int getEnable();
    void setTabRow(int);
    int getTabRow();
    void setDate(int);
    int getDate();
    void setLog(int);
    int getLog();
    void setCUS(int);
    int getCUS();
    void setIndicator(QString &);
    void getIndicator(QString &);
    void setLines(QList<PlotLine *> &);
    void getLines(QList<PlotLine *> &);
    void setSettings(Setting &);
    void getSettings(Setting &);
    void addLine (PlotLine *);
    void clear ();
    void setChartObjects(QHash<QString, COPlugin *> &);
    void getChartObjects(QHash<QString, COPlugin *> &);
    void addChartObject (COPlugin *);
    void clearChartObjects ();
    void deleteChartObject (QString &);

  protected:
    QList<PlotLine *> lines;
    QHash<QString, COPlugin *> chartObjects;
    Setting settings;
    QString name;
    QString indicator;
    int enable;
    int tabRow;
    int date;
    int log;
    int cus;
};

#endif
