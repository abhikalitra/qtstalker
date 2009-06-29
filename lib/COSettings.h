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

#ifndef COSETTINGS_HPP
#define COSETTINGS_HPP

#include <QString>
#include <QHash>
#include <QColor>
#include <QDateTime>



class COSettings
{
  public:
    COSettings (QString i, QString s, QString indi, QString t);
    COSettings ();
    ~COSettings ();
    void getString (QString &, QString &);
    void setString (QString &, QString &);
    void getColor (QColor &);
    void setColor (QColor &);
    double getValue ();
    void setValue (double);
    double getValue2 ();
    void setValue2 (double);
    void getDate (QDateTime &);
    void setDate (QDateTime &);
    void getDate2 (QDateTime &);
    void setDate2 (QDateTime &);
    void getSettings (QString &);
    void parse (QString &);
    void getIndicator (QString &);
    void setIndicator (QString &);
    void getType (QString &);
    void setType (QString &);
    void getID (QString &);
    void setID (QString &);
    void getSymbol (QString &);
    void setSymbol (QString &);
    void getText (QString &);
    void setText (QString &);
    
  protected:
    QString id;
    QString symbol;
    QString indicator;
    QString type;
    QHash<QString, QString> settings;
    QString dateFormat;
};

#endif
