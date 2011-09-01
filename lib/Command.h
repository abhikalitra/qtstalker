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

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <QWidget>

#include "Message.h"
#include "Script.h"

class Command : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage(QString);

  public:
    enum ReturnCode
    {
      _OK,
      _ERROR,
      _WAIT
    };

    Command (QObject *);
    virtual int request (Message *, Message *);
    virtual int runScript (Data *, Script *);
    virtual Data * settings ();

    QString type ();
    int isDialog ();
    QString returnString ();
    void message (QString session, QString command, QString member, QString data);

  protected:
    QString _type;
    int _isDialog;
    QString _returnString;
};

#endif
