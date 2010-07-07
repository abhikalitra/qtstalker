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

#ifndef QUOTE_SERVER_H
#define QUOTE_SERVER_H

#include <QTcpServer>
#include <QFile>

class QuoteServer : public QTcpServer
{
  Q_OBJECT

  public:
    QuoteServer (QObject *, QString &host, QString &port, QString &path, QString &log);

  public slots:
    void shuttingDown ();

  protected:
    void incomingConnection (int socketDescriptor);

  private:
    QString _dbPath;
    QFile _logFile;
};

#endif
