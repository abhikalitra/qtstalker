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

#include "GridButton.h"
#include "Globals.h"
#include "Script.h"

#include "../pics/grid.xpm"
#include "../pics/color.xpm"

#include <QSettings>

GridButton::GridButton ()
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setIcon(QIcon(grid_xpm));
  setText(tr("Chart &Grid"));
  setStatusTip(tr("Toggle the chart grid. Right click mouse for options."));
  setToolTip(tr("Toggle the chart grid"));
  setCheckable(TRUE);

  QSettings settings(g_settingsFile);
  setChecked(settings.value("grid", 1).toInt());

  _menu = new QMenu(this);
  _menu->addAction(QPixmap(color_xpm), tr("Grid &Color"), this, SLOT(colorDialog()), Qt::ALT+Qt::Key_C);
  connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(contextMenu()));

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void GridButton::changed (bool)
{
  QSettings settings(g_settingsFile);
  Script *script = new Script;
  script->setName("GridStatusChanged");
  script->setFile(settings.value("grid_status_changed_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void GridButton::colorDialog ()
{
  QSettings settings(g_settingsFile);
  Script *script = new Script;
  script->setName("GridColor");
  script->setFile(settings.value("grid_color_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void GridButton::contextMenu ()
{
  _menu->exec(QCursor::pos());
}
