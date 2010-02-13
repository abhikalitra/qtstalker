/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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


#ifndef TESTCONFIG_HPP
#define TESTCONFIG_HPP

#include <QString>

#include "BaseConfig.h"

class TestConfig : public BaseConfig
{
  public:
    enum Parm
    {
      MainWindowSize,
      MainWindowPos,
      ScriptDirectory,
      DbName, // sql db name
      DbPlugin, // the qt name for the sql db plugin used for quotes
      DbHostName, // the hostname used for sql quotes
      DbUserName, // sql quotes db user name needed for login
      DbPassword, // sql db user password
      DbAllSymbols, // the sql command to get all symbols in db
      DbSearchSymbols, // the sql command to get a symbol search
      DbFirstDate, // the sql command to get the first date for a symbol
      DbLastDate, // the sql command to get the last date for a symbol
      DbGetSymbol // the sql command to get symbol quotes
    };

    TestConfig ();
    void init (QString session); // called only at qtstalker startup, initializes tables
    void setData (Parm, QString &);
    void getData (Parm, QString &);
    void setDefaults ();
};

#endif

