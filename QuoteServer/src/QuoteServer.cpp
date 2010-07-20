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

#include "QuoteServer.h"
#include "QSRequestThread.h"
#include "QSQuoteDataBase.h"
#include "QSLog.h"
#include "QSGlobals.h"

#include <QDebug>
#include <QList>
#include <QString>
#include <QNetworkInterface>
#include <QDir>
#include <QDateTime>

QuoteServer::QuoteServer (QObject *p, QString &, QString &port, QString &path, QString &log) : QTcpServer(p)
{
  QSLog errLog;
  
//  if (host.isEmpty())
    QString host = "127.0.0.1";

  if (port.isEmpty())
    port = "5000";

  if (! listen(QHostAddress(host), (quint16) port.toInt()))
  {
    qDebug() << "Unable to start the server:" << errorString();
    return;
  }

  // create the directory structure
  if (path.isEmpty())
    path = QDir::homePath() + "/QuoteServer";

  QDir dir;
  if (! dir.mkpath(path))
  {
    qDebug() << "Error creating directory " << path;
    return;
  }
  
  _dbPath = path + "/quotes.sqlite";

  // setup log file
  if (log.isEmpty())
    log = path + "/QuoteServer.log";

  _logFile.setFileName(log);
  _logFile.open(QIODevice::Append | QIODevice::WriteOnly | QIODevice::Text);
  g_log.setDevice(&_logFile);

  // write startup message to console and log
  QString mess = " QuoteServer 0.1";
  mess.append(" running on " + serverAddress().toString());
  mess.append(" port# " + QString::number((int) serverPort()));
  mess.append(" using log file " + log);
  errLog.message(QSLog::Info, mess);
  
  qDebug() << mess;

  // initialize the quotes db
  QSQuoteDataBase db(_dbPath);
}

void QuoteServer::incomingConnection (int socketDescriptor)
{
  QSRequestThread *thread = new QSRequestThread(this, socketDescriptor, _dbPath);
  connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
  thread->start();
}

void QuoteServer::shuttingDown ()
{
  QSLog errLog;
  errLog.message(QSLog::Info, QString(" shutting down..."));

  _logFile.close();
}

