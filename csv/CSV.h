/*
 *  CSV stock quote importer
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


#ifndef CSV_H
#define CSV_H

#include "Bar.h"

#include <QStringList>
#include <QString>
#include <QDateTime>
#include <QtSql>


class CSV
{
  public:
    CSV ();
    int openDb (QString &file);
    void clear ();
    void setChart (QList<Bar> *bars);
    void import ();
    void importQuote ();
    void setFormat (QString &);
    void setDateFormat (QString &);
    void setTimeFormat (QString &);
    void setDelimiter (QString &);
    void setInputFile (QString &);
    void setDate (QString &);
    void setTime (QString &);
    void setOpen (QString &);
    void setHigh (QString &);
    void setLow (QString &);
    void setClose (QString &);
    void setVolume (QString &);
    void setOI (QString &);
    void setSymbol (QString &);
    void setDbFile (QString &);
    void setName (QString &);
    void setExchange (QString &);
    void setType (QString &);

    int setBarDate (Bar &r, QString &d);
    int setBarTime (Bar &r, QString &d);
    void setBarSymbol (Bar &r, QString &d);
    int setBarOpen (Bar &r, QString &d);
    int setBarHigh (Bar &r, QString &d);
    int setBarLow (Bar &r, QString &d);
    int setBarClose (Bar &r, QString &d);
    int setBarVolume (Bar &r, QString &d);
    int setBarOI (Bar &r, QString &d);
    int setBarName (Bar &r, QString &d);

    void convertSymbol (QString &symbol);

  private:
    QString dbFile;
    QString format;
    QString dateFormat;
    QString timeFormat;
    QString delimiter;
    QString inputFile;
    QString date;
    QString time;
    QString open;
    QString high;
    QString low;
    QString close;
    QString volume;
    QString oi;
    QString symbol;
    QString name;
    QString exchange;
    QString type;
};

#endif

