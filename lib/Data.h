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

// ******************************************************
// ******************************************************

#ifndef DATA_HPP
#define DATA_HPP

#include <QHash>

#include "Setting.h"

class Data
{
  public:
    Data ();
    ~Data ();
    int type ();
    bool dataContains (QString);
    int dataKeyCount ();
    void setCommand (QString);
    QString command ();
    void setCommandType (QString);
    QString commandType ();
    void setScriptFile (QString);
    QString scriptFile ();
    void setDeleteFlag (int);
    int deleteFlag ();

    virtual void clear ();
    virtual int set (QString, Setting *);
    virtual int set (int, Setting *);
    virtual int set (int, Data *);
    virtual Setting * get (QString);
    virtual Setting * get (int);
    virtual Data * getData (int);
    virtual int highLow (double &, double &);
    virtual QList<int> barKeys ();
    virtual QList<QString> dataKeys ();
    virtual int barKeyCount ();
    virtual void barKeyRange (int &, int &);
    virtual void append (Data *);
    virtual void setOffset (int);
    virtual int offset ();
    virtual QString toString ();
    virtual int fromString (QString);

  protected:
    QHash<QString, Setting *> _data;
    int _type;
    QString _command;
    QString _commandType;
    QString _scriptFile;
    int _delFlag;
};

#endif
