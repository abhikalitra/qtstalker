/*
 *  QtStalkerTester
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include <QTranslator>
#include <QString>
#include <QLocale>
#include <QtDebug>

#include "QtStalkerTester.h"
#include "../lib/qtstalker_defines.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  QCoreApplication::setOrganizationName("QtStalkerTester");
  QCoreApplication::setApplicationName("QtStalkerTester");
  QTranslator tor( 0 );

  QString i18nDir = QString("%1/qtstalker/i18n").arg(INSTALL_DATA_DIR);
  QString i18nFilename = QString("qtstalker_%1").arg(QLocale::system().name());
  tor.load(i18nFilename, i18nDir);
  a.installTranslator( &tor );

  QtStalkerTester *app = new QtStalkerTester;

  app->show();

  return a.exec();
}

