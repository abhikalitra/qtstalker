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

#ifndef EXSCRIPT_HPP
#define EXSCRIPT_HPP

#include "BarData.h"
#include "SCQuote.h"
#include "Indicator.h"

#include <QProcess>
#include <QList>
#include <QString>
#include <QStringList>
#include <QObject>

class ExScript : public QObject
{
  Q_OBJECT

  signals:
    void signalDone ();

  public:
    enum Function
    {
      CO, // chart object functions
      DELETE, // deletes a single indicator
      INDICATOR, // get indicator functions
      GROUP, // group database functions
      PLOT, // plot the desired indicator
      QUOTE, // quote database functions
      _SYMBOL, // symbol functions
      TEST // tester functions
    };

    ExScript ();
    ~ExScript ();
    void clear ();
    void setBarData (BarData *d);
    int calculate (QString &command);
    int calculate2 (QString &command); // used for non indicator scripts
    Indicator & indicator ();
    int getState ();
    void stop ();

  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);

  private:
    QProcess *_proc;
    QStringList _functionList;
    BarData *_data;
    int _killFlag;
    SCQuote _quotes;
    Indicator _indicator;
};

#endif
