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

  // request to server values delimited by a comma, end of text is delimited by a '\n' (newline)
  QByteArray ba;
  while (1)
  {
    if (! socket.waitForReadyRead())
    {
      log.message(QSLog::Error, QString("QSRequestThread: get server request" + QString::number(socket.error()) + socket.errorString()));
      return;
    }

    ba.append(socket.readAll());

    if (ba.contains('\n'))
      break;
  }

  ba = ba.trimmed();
//qDebug() << s;
  QString s(ba);
  QStringList l = s.split(",");

  // deal with request
  QSPluginFactory fac;
  QSPlugin *plug = fac.plugin(l[0]);
  if (! plug)
  {
    log.message(QSLog::Error, QString("QSRequestThread: invalid plugin" + l.at(0)));
    return;
  }

  // run plugin
  s.clear();
  plug->command(l, _dbPath, s);
  s.append("\n");
//qDebug() << "response" << response;

  // check if we are still connected
  if (socket.error() == QAbstractSocket::RemoteHostClosedError ||
      socket.state() != QAbstractSocket::ConnectedState)
    return;

  // send the response
  ba.clear();
  ba.append(s);
  socket.write(ba);
  while (socket.bytesToWrite())
  {
    if (! socket.waitForBytesWritten())
    {
      log.message(QSLog::Error, QString("QSRequestThread: send response" + QString::number(socket.error()) + socket.errorString()));
      return;
    }
  }

  // wait for client to disconnect
  socket.waitForDisconnected();
}

// test data compression 
/*
void QSRequestThread::run()
{
  QSLog log;
  QTcpSocket socket;
  if (! socket.setSocketDescriptor(_socketDescriptor))
  {
    log.message(QSLog::Error, QString("QSRequestThread: " + QString::number(socket.error()) + socket.errorString()));
    return;
  }

  // we first get the data package description which is plain text
  // 2 values delimited by a comma, end of text is delimited by a '\n' (newline)
  // first value is either a 'Z' for zlib compressed text or 'T' for plain text
  // second value is the total number of bytes to receive
  QByteArray ba;
  while (1)
  {
    if (! socket.waitForReadyRead(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: get package description" + QString::number(socket.error()) + socket.errorString()));
      return;
    }

    ba.append(socket.readAll());
    
    if (ba.contains('\n'))
      break;
  }

  ba = ba.trimmed();
//qDebug() << details;
  QList<QByteArray> l = ba.split(',');
  if (l.count() != 2)
  {
    log.message(QSLog::Error, QString("QSRequestThread: error in data package description" + ba));
    return;
  }

  // verify the package type
  int zFlag = 0;
  if (l[0].at(0) == 'Z')
    zFlag = 1;
  else
  {
    if (l[0].at(0) == 'T')
      zFlag = 0;
    else
    {
      log.message(QSLog::Error, QString("QSRequestThread: error in data package description type" + l[0]));
      return;
    }
  }

  // verify package size
  bool ok;
  int size = l[1].toInt(&ok);
  if (! ok)
  {
    log.message(QSLog::Error, QString("QSRequestThread: error in data package size" + l[1]));
    return;
  }    

  // now we get the package
  ba.clear();
  while (1)
  {
    if (! socket.waitForReadyRead(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: get package" + QString::number(socket.error()) + socket.errorString()));
      return;
    }

    ba.append(socket.readAll());

    if (ba.count() == size)
      break;
  }

  // now we inflate package if needed
  if (zFlag)
    ba = qUncompress(ba);

  QString data(ba);
  QStringList sl = data.split(",");

  // deal with request
  QSPluginFactory fac;
  QSPlugin *plug = fac.plugin(sl[0]);
  if (! plug)
  {
    log.message(QSLog::Error, QString("QSRequestThread: invalid plugin" + sl.at(0)));
    return;
  }
  
  // run plugin
  QString response;
  plug->command(sl, _dbPath, response);
//  response.append("\n");
//qDebug() << "response" << response;

  // check if we are still connected
  if (socket.error() == QAbstractSocket::RemoteHostClosedError ||
      socket.state() != QAbstractSocket::ConnectedState)
    return;

  // compress the response
  ba.clear();
  ba.append(response);
  ba = qCompress(ba);

  // send the response package description
  QByteArray ba2;
  ba2.append("Z," + QString::number(ba.count()) + "\n");

  socket.write(ba2);
  while (socket.bytesToWrite())
  {
    if (! socket.waitForBytesWritten(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: send response package description" + QString::number(socket.error()) + socket.errorString()));
      return;
    }
  }

  // send the response package
  socket.write(ba);
  while (socket.bytesToWrite())
  {
    if (! socket.waitForBytesWritten(_timeOut))
    {
      log.message(QSLog::Error, QString("QSRequestThread: send response package" + QString::number(socket.error()) + socket.errorString()));
      return;
    }
  }

  // wait for client to disconnect
  socket.waitForDisconnected(_timeOut);
}
*/

