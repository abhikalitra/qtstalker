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

#include "QSRequestThread.h"
#include "QSPluginFactory.h"
#include "QSLog.h"

#include <QtNetwork>

QSRequestThread::QSRequestThread (QObject *p, int sd, QString &dbPath) : QThread(p)
{
  _socketDescriptor = sd;
  _dbPath = dbPath;
  _timeOut = 5000;
}

void QSRequestThread::run()
{
  QSLog log;
  QTcpSocket socket;
  if (! socket.setSocketDescriptor(_socketDescriptor))
  {
    log.message(QSLog::Error, QString("QSRequestThread: " + QString::number(socket.error()) + socket.errorString()));
    return;
  }

  // keep reading data until we receive a newline char
  // signifies end of command
  QString data;
  while (1)
  {
    if (! socket.waitForReadyRead(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: " + QString::number(socket.error()) + socket.errorString()));
      return;
    }

    QByteArray ba = socket.readAll();
    QString s(ba);
    if (s.contains("\n"))
    {
      data.append(s);
      break;
    }

    data.append(s);
  }

  data = data.trimmed();
//qDebug() << data;
  QStringList l = data.split(",");

  // deal with request
  QSPluginFactory fac;
  QSPlugin *plug = fac.plugin(l[0]);
  if (! plug)
  {
    log.message(QSLog::Error, QString("QSRequestThread: invalid plugin" + l.at(0)));
    return;
  }
  
  // run plugin
  QString response;
  plug->command(l, _dbPath, response);
  response.append("\n");
//qDebug() << "response" << response;

  // check if we are still connected
  if (socket.error() == QAbstractSocket::RemoteHostClosedError ||
      socket.state() != QAbstractSocket::ConnectedState)
    return;

  // write repsone to socket
  QByteArray block;
  block.append(response);
  socket.write(block);

  // wait for bytes to be written
  while (socket.bytesToWrite())
  {
    if (! socket.waitForBytesWritten(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: " + QString::number(socket.error()) + socket.errorString()));
      return;
    }
  }

  // wait for client to disconnect
  socket.waitForDisconnected(_timeOut);
}

