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
#include "Config.h"
#include "../pics/nav.xpm"

SidePanelButton::SidePanelButton ()
{
  setIcon(QIcon(nav));
  setText(tr("Show / Hide Side Panel"));
  setStatusTip(tr("Show / Hide Side Panel"));
  setToolTip(tr("Show / Hide Side Panel"));
  setCheckable(TRUE);

  Config config;
  QString s;
  config.getData(Config::SidePanelStatus, s);

  bool ok;
  int status = s.toInt(&ok);
  if (ok)
    setChecked(status);
  else
  {
    setChecked(TRUE);
    config.setData(Config::SidePanelStatus, 1);
  }

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void SidePanelButton::changed (bool status)
{
  Config config;
  config.setData(Config::SidePanelStatus, status);

  emit signalChanged(status);
}

