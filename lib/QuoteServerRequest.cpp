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


#include "QuoteServerRequest.h"
#include "Config.h"

#include <QTcpSocket>
#include <QtNetwork>

QuoteServerRequest::QuoteServerRequest ()
{
  _timeOut = 5000;
}

int QuoteServerRequest::run (QString &request)
{
  Config config;
  QString serverName;
  config.getData(Config::QuoteServerName, serverName);

  int serverPort = config.getInt(Config::QuoteServerPort);
  
  QTcpSocket socket;
  socket.connectToHost(serverName, (quint16) serverPort);
  if (! socket.waitForConnected())
  {
    qDebug() << "QuoteServerRequest::run: connectToHost" << socket.error() << socket.errorString();
    return 1;
  }

  // write request to socket
  QByteArray block;
  block.append(request);
  socket.write(block);

  // wait for bytes to be written
  while (socket.bytesToWrite())
  {
    if (! socket.waitForBytesWritten())
    {
      qDebug() << "QuoteServerRequest::run: bytesToWrite" << socket.error() << socket.errorString();
      return 1;
    }
  }

  // now we wait for a response from the server
  // keep reading data until we receive a newline char
  // signifies end of command
  _data.clear();
  
  while (1)
  {
    if (! socket.waitForReadyRead())
    {
      qDebug() << "QuoteServerRequest::run: waitForReadyRead" << socket.error() << socket.errorString();
      return 1;
    }

    QByteArray ba = socket.readAll();
    QString s(ba);
    _data.append(s);

    if (s.contains("\n"))
      break;
  }

  _data = _data.trimmed();
//qDebug() << data;
  
  socket.disconnectFromHost();

  return 0;
}

QString & QuoteServerRequest::data ()
{
  return _data;
}

