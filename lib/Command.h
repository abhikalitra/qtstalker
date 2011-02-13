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
// *************************************************************************************************

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <QStringList>
#include <QByteArray>
#include <QHash>

#include "Indicator.h"
#include "BarData.h"

class Command
{
  public:
    Command ();
    Command (QString);
    void clear ();
    void parse (QString);
    QString plugin ();
    void setReturnData (QString, QString);
    QString returnData (QString);
    QByteArray arrayReturnData (QString);
    Indicator * indicator ();
    void setIndicator (Indicator *);
    BarData * barData ();
    void setBarData (BarData *);
    void strip (QString &);
    int count ();
    QString parm (QString);
    void setReturnCode (QString);
    QByteArray returnCode ();
    void setName (QString);
    QString name ();

  private:
    QHash<QString, QString> _parms;
    QHash<QString, QString> _returnData;
    Indicator *_indicator;
    BarData *_barData;
    QString _returnCode;
    QString _name;
};

#endif
