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

#include "MessageServer.h"
#include "Message.h"
#include "Globals.h"

#include <QDebug>
#include <QDataStream>
#include <QString>

MessageServer::MessageServer (QObject *parent) : QObject(parent)
{
  _socket = 0;
  _server = new QLocalServer(this);

  QString serverName = "QtStalkerMessageServer" + g_session;
  
  QLocalServer::removeServer(serverName);
  
  if (! _server->listen(serverName))
    qDebug() << "MessageServer::MessageServer:" << _server->errorString();

  connect(_server, SIGNAL(newConnection()), this, SLOT(socketNewConnection()));
}

MessageServer::~MessageServer ()
{
}

void MessageServer::socketNewConnection ()
{
  _socket = _server->nextPendingConnection();

  while (_socket->bytesAvailable() < (int)sizeof(quint32))
    _socket->waitForReadyRead();

  connect(_socket, SIGNAL(disconnected()), _socket, SLOT(deleteLater()));

  QDataStream in(_socket);
  in.setVersion(QDataStream::Qt_4_0);
  if (_socket->bytesAvailable() < (int) sizeof(quint16))
    return;

  QString message;
  in >> message;

  switch ((Message::Type) message.toInt())
  {
    case Message::_REFRESH_CHART_PANEL:
      emit signalRefreshChartPanel();
      break;
    case Message::_REFRESH_GROUP_PANEL:
      emit signalRefreshGroupPanel();
      break;
    case Message::_REFRESH_SCRIPT_PANEL:
      emit signalRefreshScriptPanel();
      break;
    default:
      break;
  }
}

/*
QLocalSocket * MessageServer::socket ()
{
  return _clientConnection;
}
*/

