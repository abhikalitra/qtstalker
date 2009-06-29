/***************************************************************************
                          main.cpp  -  
                             -------------------
    begin                : Thu Mar  7 22:43:41 EST 2002
    copyright            : (C) 2001-2008 by Stefan Stratigakos
    email                :
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QTranslator>
#include <QString>
#include <QLocale>

#include "Qtstalker.h"
#include "../lib/qtstalker_defines.h"


int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
#ifdef MINGW
  QCoreApplication::setOrganizationName("QtStalker Team");
  QCoreApplication::setApplicationName("QtStalker");
#endif
  QTranslator tor( 0 );

  QString i18nDir = QString("%1/qtstalker/i18n").arg(INSTALL_DATA_DIR);
  QString i18nFilename = QString("qtstalker_%1").arg(QLocale::system().name());
  tor.load(i18nFilename, i18nDir);
  a.installTranslator( &tor );

  QtstalkerApp *qtstalker = new QtstalkerApp();

  qtstalker->show();

  return a.exec();
}

