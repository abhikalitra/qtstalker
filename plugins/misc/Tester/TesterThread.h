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

#ifndef TESTER_THREAD_HPP
#define TESTER_THREAD_HPP

#include <QThread>
#include <QStringList>
#include <QList>

#include "Indicator.h"
#include "TesterSettings.h"
#include "BarData.h"
#include "TesterTrade.h"

struct PlotRule
{
  int enable;
  QString name;
  int op;
  QString value;
};

class TesterThread : public QThread
{
  Q_OBJECT

  signals:
    void signalMessage (QString);
    void signalDone (QString, QStringList);
    
  public:
    enum Status
    {
      _None,
      _Long,
      _Short,
      _EnterLong,
      _EnterShort,
      _ExitTrade
    };
    
    TesterThread (QObject *, TesterSettings &);
    void stop ();
    int getBars (BarData &);
    int getIndicator (BarData &, QStringList &);
    int getRules (QStringList &, BarData &, QList<PlotRule> &);
    int enterTradeCheck (QList<PlotRule> &, int index);
    int enterTrade (QList<TesterTrade> &, BarData &, int index, int status);
    void exitTrade (QList<TesterTrade> &, BarData &, int index);
    double getCommission (TesterTrade &, int flag);
    void getReport (QList<TesterTrade> &, QString &, QStringList &);

  protected:
    void run();

  private:
    Indicator _indicator;
    TesterSettings _settings;
    int _stopFlag;
};

#endif
