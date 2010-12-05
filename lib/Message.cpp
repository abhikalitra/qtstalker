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

#include "Message.h"
#include "Globals.h"

#include <QDebug>
#include <QDataStream>
#include <QLocalSocket>
#include <QObject>

Message::Message ()
{
  _method << "REFRESH_CHART_PANEL" << "REFRESH_GROUP_PANEL" << "REFRESH_SCRIPT_PANEL";
}

/*
int Message::newConnection (QLocalSocket *socket)
{
  socket->abort();

  QString server = "QtStalkerMessageServer" + g_session;
  socket->connectToServer(server);
  if (! socket->waitForConnected(1000))
  {
    qDebug() << "Message::newConnection:" << socket->errorString();
    return 1;
  }

  connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
  
  return 0;
}
*/

int Message::sendMessage (Message::Type type)
{
  QLocalSocket *socket = new QLocalSocket;
  QObject::connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
  socket->abort();

  QString server = "QtStalkerMessageServer" + g_session;
  socket->connectToServer(server);
  if (! socket->waitForConnected(1000))
  {
    qDebug() << "Message::sendMessage:" << socket->errorString();
    return 1;
  }

  QByteArray block;
  QDataStream out(&block, QIODevice::WriteOnly);
  out.setVersion(QDataStream::Qt_4_0);
  out << QString::number(type);
  out.device()->seek(0);
  socket->write(block);
  socket->flush();

  return 0;
}

/*
void Message::receiveMessage (QLocalSocket *socket, QString &message)
{
  // receive message from message server
  message.clear();
  
  while (socket->bytesAvailable() < (int)sizeof(quint32))
    socket->waitForReadyRead();

  connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));

  QDataStream in(socket);
  in.setVersion(QDataStream::Qt_4_0);
  if (socket->bytesAvailable() < (int) sizeof(quint16))
    return;

  in >> message;
}
*/

int Message::stringToType (QString d)
{
  return _method.indexOf(d);
}

