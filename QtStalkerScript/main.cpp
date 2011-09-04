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

#include <QApplication>
#include <QString>
#include <QtDebug>
#include <QDBusConnection>

#include "QtStalkerScript.h"
#include "../lib/qtstalker_defines.h"

int main(int argc, char *argv[])
{
  if (argc != 4)
  {
    qDebug() << "QtStalkerScript::main: invalid args" << argc;
    exit(1);
  }

  int pos = 1;
  QString session = argv[pos++];
  QString command = argv[pos++];
  QString file = argv[pos++];

  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("QtStalkerScript");
  QCoreApplication::setApplicationName("QtStalkerScript");

  if (! QDBusConnection::sessionBus().isConnected())
  {
    qWarning("Cannot connect to the D-Bus session bus.\n"
             "Please check your system settings and try again.\n");
    return 1;
  }

  QtStalkerScript app(session, command, file);
  return a.exec();
}
