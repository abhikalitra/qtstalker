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

#include "CrossHairsButton.h"
#include "Globals.h"
#include "Script.h"

#include "../pics/crosshair.xpm"
#include "../pics/color.xpm"

#include <QString>
#include <QDebug>
#include <QIcon>
#include <QSettings>

CrossHairsButton::CrossHairsButton ()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setIcon(QIcon(crosshair_xpm));
  setText(tr("Cursor Crosshairs"));
  setStatusTip(tr("Toggle the cursor crosshairs. Right click mouse for options."));
  setToolTip(tr("Toggle the cursor crosshairs"));
  setCheckable(TRUE);
  setShortcut(QKeySequence(Qt::CTRL + Qt::Key_H));

  QSettings settings(g_localSettings);
  setChecked(settings.value("crosshairs", 0).toInt());

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(color_xpm), tr("Crosshair &Color"), this, SLOT(dialog()), Qt::ALT+Qt::Key_C);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));
}

void CrossHairsButton::changed (bool)
{
  QSettings settings(g_globalSettings);
  Script *script = new Script(this);
  script->setName("CrosshairsStatusChanged");
  script->setFile(settings.value("crosshairs_status_changed_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void CrossHairsButton::dialog ()
{
  QSettings settings(g_globalSettings);
  Script *script = new Script(this);
  script->setName("CrosshairsColor");
  script->setFile(settings.value("crosshairs_color_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void CrossHairsButton::contextMenu ()
{
  _menu->exec(QCursor::pos());
}
