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

// *************************************************************************************************
// Script interface for qtstalker scripts during runtime. Serves as a IPC between script and
// qtstalker to route data back and forth. all scripts that run use this to execute.
// *************************************************************************************************

#ifndef EXSCRIPT_HPP
#define EXSCRIPT_HPP

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
      _CO, // chart object functions
      _INDICATOR, // indicator functions
      _GROUP, // group database functions
      _PROCESS, // start a detached system process
      _SYMBOL, // symbol functions
      _TEST // tester functions
    };

    ExScript ();
    ~ExScript ();
    void clear ();
    void setIndicator (Indicator &);
    int calculate (QString &command); // blocking version
    int calculate2 (QString &command); // no blocking version
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
    int _killFlag;
    Indicator _indicator;
};

#endif
