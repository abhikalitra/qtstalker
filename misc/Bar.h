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

#ifndef BAR_HPP
#define BAR_HPP

#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QHash>



class Bar
{
  public:
    Bar ();
    ~Bar ();
    int setDate (QDateTime &);
    void getDate (QDateTime &);
    bool getEmptyFlag ();
    void getDateString (QString &);
    void getDateTimeString (QString &);
    void getTimeString (QString &);
    void clear ();
    void setSymbol (QString &);
    void getSymbol (QString &);
    void setTime (QTime &);
    void setData (QString &, QString &);
    void getData (QString &, QString &);
    bool getValidDate ();
    void getDateNumber (QString &);
    void getDataString (QString &format, QString &s);
    
  protected:
    QHash<QString, QString> data;
    QDateTime date;
    bool emptyFlag;
};

#endif
