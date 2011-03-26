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

#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include "Indicator.h"
#include "BarData.h"
#include "PluginFactory.h"
#include "Command.h"

#include <QProcess>
#include <QList>
#include <QString>
#include <QStringList>
#include <QObject>
#include <QHash>
#include <QWidget>

class Script : public QObject
{
  Q_OBJECT

  signals:
    void signalDone (QString);
    void signalMessage (QString);
    void signalStopped (QString);
    void signalKill ();

  public:
    Script (QWidget *);
    Script (QObject *);
    Script ();
    ~Script ();
    void init ();
    void clear ();
    void setIndicator (Indicator *);
    Indicator * indicator ();
    void setBarData (BarData *);
    void setName (QString);
    QString & name ();
    void setCommand (QString);
    QString & command ();
    void setFile (QString);
    QString & file ();
    void setMinutes (int);
    int minutes ();
    void setLastRun (QString);
    QString & lastRun ();
    void setType (QString);
    QString & type ();
    void setComment (QString);
    QString & comment ();
    
  public slots:
    void readFromStdout ();
    void readFromStderr ();
    void done (int, QProcess::ExitStatus);
    int startScript ();
    void stopScript (QString);
    void resume ();

  private:
    QProcess *_proc;
    int _killFlag;
    Indicator *_indicator;
    int _indicatorFlag;
    BarData *_barData;
    PluginFactory _factory;
    QString _name;
    QString _com;
    QString _file;
    QString _type;
    QString _comment;
    int _minutes;
    QString _lastRun;
    QHash<QString, Plugin *> _plugins;
    Command *_command;
    QWidget *_parent;
};

#endif
