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
#include "../pics/qtstalker.xpm"
#include "Dialog.h"
#include "Globals.h"

#include <QApplication>
#include <QDesktopServices>
#include <QString>
#include <QDebug>
#include <QCursor>

DocsAction::DocsAction ()
{
  _menu = new QMenu(this);
  _menu->addAction(QPixmap(help), tr("&Documentation"), this, SLOT(startDocumentation()), Qt::ALT+Qt::Key_D);
  _menu->addAction(QPixmap(qtstalker), tr("&About QtStalker"), this, SLOT(about()), Qt::ALT+Qt::Key_A);
  
  _assistant = new Assistant;

  setIcon(QIcon(help));
  setText(tr("&Help"));
  setStatusTip(tr("Show documentation. Right click mouse for options."));
  setToolTip(tr("Show documentation."));
  connect(this, SIGNAL(clicked()), this, SLOT(startDocumentation()));
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

  _assistant->showDocumentation("index.html");
}

void DocsAction::about ()
{
  QString versionString = "Qtstalker version 0.37-dev (working title)";
  versionString.append("\nBuilt using Qt ");
  versionString += QT_VERSION_STR;
  versionString.append("\n(C) 2001-2010 by Stefan Stratigakos\nqtstalker.sourceforge.net");
  versionString.append("\n\nQtstalker is licensed with GNU General Public License (GPL) version 2.");
  versionString.append("\nQt Assistant is licensed with GNU General Public License (GPL) version 3.");

  Dialog *dialog = new Dialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("About"));

  QFont f = dialog->messageFont();
  f.setBold(FALSE);
  dialog->setMessageFont(f);

  dialog->setMessage(versionString);
  
  dialog->setIcon(qtstalker);
  dialog->show();
}

void DocsAction::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}

