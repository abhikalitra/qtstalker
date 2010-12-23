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

#include "ChartBackgroundColorButton.h"
#include "Globals.h"
#include "Script.h"

#include "../pics/color.xpm"

#include <QDebug>
#include <QString>
#include <QSettings>

ChartBackgroundColorButton::ChartBackgroundColorButton ()
{
  setIcon(QIcon(color_xpm));
  setStatusTip(tr("Chart Background Color"));
  setToolTip(tr("Chart Background Color"));
  
  connect(this, SIGNAL(clicked()), this, SLOT(dialog()));
}

void ChartBackgroundColorButton::dialog ()
{
  QSettings settings(g_settingsFile);
  Script *script = new Script;
  script->setName("ChartBackgroundColor");
  script->setFile(settings.value("chart_background_color_script").toString());
  script->setCommand("perl");
  script->startScript();
}
