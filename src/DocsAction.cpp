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

#include "DocsAction.h"
#include "../pics/help.xpm"

#include <QApplication>
#include <QDesktopServices>
#include <QString>
#include <QDebug>

DocsAction::DocsAction (QObject *p) : QAction (p)
{
  _assistant = new Assistant;

  setIcon(QIcon(help));
  setText(tr("&Help"));
  setStatusTip(tr("Show documentation."));
  setToolTip(tr("Show documentation."));
  connect(this, SIGNAL(triggered()), this, SLOT(startDocumentation()));
}

DocsAction::~DocsAction ()
{
  delete _assistant;
}

void DocsAction::startDocumentation ()
{
/*
FIXME: Due to the Qt issue 262508 (see docs/docs.html) we need to show them
how to remove the stale cache file. This is complicated to report the location
on different OSs (but perhaps i do not understand).
This workaround should all go away when the Qt bug is fixed, but only if we
raise the minimum Qt version.
*/
  QString location = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#ifdef Q_WS_MAC
  location.insert(location.count() - QCoreApplication::applicationName().count(),
    QCoreApplication::organizationName() + "/");
#endif
  qDebug("DocsAction::startDocumentation: Documentation cache: %s/", qPrintable(location));

  // start _assistant
  _assistant->showDocumentation("index.html");
}

