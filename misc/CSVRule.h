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

#ifndef CSVRULE_HPP
#define CSVRULE_HPP

#include <QString>
#include <QStringList>


class CSVRule
{
  public:
    CSVRule ();
    ~CSVRule ();
    void setName (QString &);
    void getName (QString &);
    void setFormat (QString &);
    void getFormat (QString &);
    void setDelimiter (QString &);
    void getDelimiter (QString &);
    void setFileNameSymbol (bool);
    bool getFileNameSymbol ();
    void setDateFormat (QString &);
    void getDateFormat (QString &);
    void getInterval (QString &);
    void setInterval (QString &);
    void getFileList (QStringList &);
    void setFileList (QStringList &);
    void getFieldList (QStringList &);
    void getBarFormat (QString &d);
    void setTimeFormat (QString &);
    void getTimeFormat (QString &);
    
  protected:
    QString name;
    QString format;
    QString delimiter;
    QString dateFormat;
    bool useFileName;
    QStringList fileList;
    QString interval;
    QString timeFormat;
};

#endif
