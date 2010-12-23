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

#include "QuitButton.h"
#include "Globals.h"

#include "../pics/done.xpm"

#include <QDebug>
#include <QString>
#include <QApplication>

QuitButton::QuitButton ()
{
  setIcon(QIcon(done_xpm));
  setStatusTip(tr("Quit") + " QtStalker");
  setToolTip(tr("Quit") + " QtStalker");
  setShortcut(QKeySequence(Qt::CTRL+Qt::Key_Q));
  connect(this, SIGNAL(clicked()), qApp, SLOT(quit()));
  connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(shutdown()));
}

void QuitButton::shutdown ()
{
  // save everything that needs to be saved before app quits
  // some classes cannot save in their destructors due to SQL services
  // shutting down before the destructor is called
  // also need to save splitter sizes properly
  emit signalShutdown();

  // delete all the non-parented objects
  delete g_barData;
  delete g_middleMan;
}
