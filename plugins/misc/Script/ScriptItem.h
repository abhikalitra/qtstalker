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

#ifndef SCRIPT_ITEM_HPP
#define SCRIPT_ITEM_HPP

#include <QString>
#include <QDateTime>
#include <QObject>

#include "ExScript.h"

class ScriptItem : public QObject
{
  Q_OBJECT
  
  signals:
    void signalDone (QString);
    void signalMessage (QString);
    void signalStopped (QString);
  
  public:
    ScriptItem ();
    void setName (QString);
    QString & name ();
    void setCommand (QString);
    QString & command ();
    void setFile (QString);
    QString & file ();
    void setComment (QString);
    QString & comment ();
    void setStatus (int);
    int status ();
    void setLastRun (QDateTime);
    QDateTime & lastRun ();
    void setType (QString);
    QString & type ();

  public slots:
    void stop ();
    void start ();
    void done ();

  private:
    QString _name;
    QString _command;
    QString _file;
    QString _comment;
    QString _type;
    int _status;
    QDateTime _lastRun;
    int _stopFlag;
    ExScript _scriptServer;
};

#endif
