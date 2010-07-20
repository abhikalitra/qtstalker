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

#ifndef YAHOO_HISTORICAL_HPP
#define YAHOO_HISTORICAL_HPP

#include <QStringList>
#include <QByteArray>
#include <QDateTime>

#include "YahooUrlData.h"
#include "QSPlugin.h"
#include "QSLog.h"

class YahooHistorical : public QSPlugin
{
  Q_OBJECT

  public:
    enum Method
    {
      _History,
      _Details
    };

    YahooHistorical ();
    int command (QStringList &input, QString &dbPath, QString &output);
    int history (QStringList &input, QString &dbPath, QString &output, QSLog &);
    int details (QStringList &input, QString &dbPath, QString &output, QSLog &);
    void url (QDateTime &sd, QDateTime &ed, YahooUrlData &);
    void urlDetails (YahooUrlData &);
    void parse (QByteArray &, QString &dbPath, YahooUrlData &);
    void parseDetails (QByteArray &, QString &dbPath, YahooUrlData &, QSLog &);

  private:
    QStringList _methodList;
    QStringList _yahooList;
};

extern "C"
{
  QSPlugin * createPlugin ();
}

#endif
