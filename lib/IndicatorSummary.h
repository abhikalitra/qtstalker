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

#ifndef INDICATORSUMMARY_HPP
#define INDICATORSUMMARY_HPP

#include <QString>
#include <QStringList>
#include <QList>
#include <QObject>
#include "BarData.h"
#include "IndicatorPlugin.h"


class IndicatorSummary : public QObject
{
  Q_OBJECT

  signals:
    void signalWakeup();

  public:
    IndicatorSummary (QStringList &, int, BarData::BarLength);
    ~IndicatorSummary ();
    void run ();
    void loadIndicators ();
    void createDataWindow (QList<Setting *> &, QString &group);

  private:
    QString helpFile;

  protected:
    QStringList indicatorList;
    int minBars;
    BarData::BarLength barLength;
    QList<IndicatorPlugin *> indicators;
};

#endif
