/*
 *  QuoteServer
 *
 *  Copyright (C) 2010 Stefan S. Stratigakos
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

#ifndef DETAILS_HPP
#define DETAILS_HPP

#include <QStringList>

#include "QSPlugin.h"
#include "QSLog.h"

class Details : public QSPlugin
{
  public:
    enum Method
    {
      _Get,
      _Set,
      _Remove
    };

    Details ();
    int command (QStringList &input, QString &dbPath, QString &output);
    int set (QStringList &input, QString &dbPath, QString &output, QSLog &);
    int get (QStringList &input, QString &dbPath, QString &output, QSLog &);
    int remove (QStringList &input, QString &dbPath, QString &output, QSLog &);

  private:
    QStringList _methodList;
};

extern "C"
{
  QSPlugin * createPlugin ();
}

#endif
