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

#include <QObject>
#include <QStringList>
#include <QWidget>

#include "Message.h"
#include "Script.h"
#include "Setting.h"
#include "Data.h"

class Command : public QObject
{
  Q_OBJECT

  signals:
    void signalMessage (Data *);
    void signalResume (void *);

  public:
    enum ReturnCode
    {
      _OK,
      _ERROR
    };

    enum Type
    {
      _NORMAL,
      _DIALOG,
      _THREAD
    };

    Command (QObject *);
    ~Command ();
    virtual int runScript (Message *, Script *);

    QString name ();
    int type ();
    QString returnString ();
    void setWidgetParent (QWidget *);

  protected:
    int _type;
    QString _name;
    QString _returnString;
    QStringList _message;
    QWidget *_widgetParent;
};

#endif
