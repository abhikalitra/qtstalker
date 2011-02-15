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

#include "HelpButton.h"
#include "Globals.h"
#include "Doc.h"
#include "AboutDialog.h"

#include "../pics/about.xpm"
#include "../pics/help.xpm"

#include <QApplication>
#include <QDesktopServices>
#include <QString>
#include <QDebug>
#include <QCursor>

HelpButton::HelpButton ()
{
  createMenu();

  setPopupMode(QToolButton::InstantPopup);
  setIcon(QIcon(help_xpm));
  setText(tr("&Help"));
  setStatusTip(tr("Show documentation"));
  setToolTip(tr("Show documentation"));
}

void HelpButton::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Help"));
  setMenu(_menu);

  // help dialog
  QAction *a = _menu->addAction(tr("Help"));
  a->setCheckable(FALSE);
  a->setIcon(QIcon(help_xpm));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(startDocumentation()));

  // about dialog
  a = _menu->addAction(tr("About"));
  a->setCheckable(FALSE);
  a->setIcon(QIcon(about_xpm));
  connect(a, SIGNAL(triggered(bool)), this, SLOT(aboutDialog()));
}

void HelpButton::startDocumentation ()
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

  Doc *doc = new Doc;  
  doc->showDocumentation("index.html");
}

void HelpButton::aboutDialog ()
{
  AboutDialog *dialog = new AboutDialog;
  dialog->show();
}
