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

#ifndef SCRIPT_HPP
#define SCRIPT_HPP

#include <QString>
#include <QDateTime>
#include <QThread>

class Script : public QThread
{
  Q_OBJECT
  
  signals:
    void signalDone (QString);
    void signalMessage (QString);
  
  public:
    Script ();
    void setName (QString &);
    QString & getName ();
    void setCommand (QString &);
    QString & getCommand ();
    void setFile (QString &);
    QString & getFile ();
    void setComment (QString &);
    QString & getComment ();
    void setStatus (int);
    int getStatus ();
    void setLastRun (QDateTime &);
    QDateTime & getLastRun ();
    void setRefresh (int);
    int getRefresh ();
    void stop ();
    
  protected:
    void run ();

  private:
    QString _name;
    QString _command;
    QString _file;
    QString _comment;
    int _status;
    QDateTime _lastRun;
    int _refresh;
    int _stopFlag;
};

#endif
