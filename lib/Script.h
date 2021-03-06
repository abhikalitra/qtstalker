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

#include <QStringList>
#include <QHash>
#include <QThread>
#include <QList>
#include <QProcess>

#include "Entity.h"
#include "Command.h"

class Script : public QThread
{
  Q_OBJECT

  signals:
    void signalDone (QString);
    void signalDeleted (QString);
    void signalMessage (QString);

  public:
    enum ReturnCode
    {
      _OK,
      _CANCEL,
      _TIMEOUT,
      _ERROR
    };
    
    Script (QObject *);
    ~Script ();
    void setData (QString, Entity &);
    int data (QString, Entity &);
    void setScriptCommand (QString, Command *);
    Command * scriptCommand (QString);
    void setName (QString);
    QString name ();
    void setFile (QString);
    QString & file ();
    int count ();
    QList<QString> dataKeys ();
    void setCommand (QString);
    QString command ();
    QString id ();
    void run ();
    void threadMessage (QString);

  public slots:
    void done (int);
    void stopScript ();

  protected:
    QProcess *_proc;
    QString _name;
    QString _file;
    QString _command;
    QHash<QString, Entity> _data;
    QHash<QString, Command *> _commands;
    int _killFlag;
    QString _id;
};

#endif
