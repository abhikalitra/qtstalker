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

#include "SidePanelButton.h"
#include "Globals.h"
#include "../pics/nav.xpm"

#include <QSettings>

SidePanelButton::SidePanelButton ()
{
  setIcon(QIcon(nav));
  setText(tr("Show / Hide Side Panel"));
  setStatusTip(tr("Show / Hide Side Panel"));
  setToolTip(tr("Show / Hide Side Panel"));
  setCheckable(TRUE);

  QSettings settings;
  settings.beginGroup("main" + g_session);

  setChecked(settings.value("side_panel_status", 1).toInt());

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void SidePanelButton::changed (bool status)
{
  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("side_panel_status", status);
  settings.sync();

  emit signalChanged(status);
}

