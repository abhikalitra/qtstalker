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

#include <QCoreApplication>
#include <QStringList>
#include <QDir>
#include <QDebug>

#include "QuoteServer.h"

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);

  QString usage = "QuoteServer (v 0.1) Usage:";
  usage.append("\n-p port # (default 5000)");
  usage.append("\n-d storage directory (default ~/QuoteServer)");
  usage.append("\n-l log (default ~/QuoteServer/QuoteServer.log)");
  usage.append("\n-h this help\n");
  
  // parse command line switches
  QStringList l = app.arguments();

  QString port, path, log;
  int loop = 1;
  for (; loop < l.count(); loop++)
  {
    QString swtch = l.at(loop);
    loop++;
    if (loop >= l.count())
    {
      qDebug() << "Missing " << swtch << "arg...aborting";
      qDebug() << usage;
      exit(1);
    }
    
    if (swtch == "-p")
    {
      port = l.at(loop);
      continue;
    }
    else if (swtch == "-d")
    {
      path = l.at(loop);
      continue;
    }
    else if (swtch == "-l")
    {
      log = l.at(loop);
      continue;
    }
    else if (swtch == "-h")
    {
      qDebug() << usage;
      exit(1);
    }

    qDebug() << "Unrecogized switch " << swtch << "...aborting";
    qDebug() << usage;
    exit(1);
  }
  
  QuoteServer server(0, port, port, path, log);
  QObject::connect(&app, SIGNAL(aboutToQuit()), &server, SLOT(shuttingDown()));
  return app.exec();
}

