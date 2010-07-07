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

#ifndef YAHOO_QUOTES_HPP
#define YAHOO_QUOTES_HPP

#include <QStringList>
#include <QByteArray>
#include <QDateTime>

#include "YahooUrlData.h"
#include "Plugin.h"

class YahooQuotes : public Plugin
{
  Q_OBJECT

  public:
    YahooQuotes ();
    int command (QStringList &input, QString &dbPath, QString &output);
    int startUpdate (QStringList &input, QString &dbPath, QString &output);
    int stopUpdate (QString &output);
    void url (QStringList &symbols, YahooUrlData &data);
    void parse (QByteArray &, QString &dbPath);
};

extern "C"
{
  Plugin * createPlugin ();
}

#endif
