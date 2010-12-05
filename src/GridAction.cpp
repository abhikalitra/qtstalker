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

#include "GridAction.h"
#include "../pics/grid.xpm"
#include "Globals.h"

#include <QColorDialog>
#include <QSettings>

GridAction::GridAction ()
{
  setIcon(QIcon(gridicon));
  setText(tr("Chart &Grid"));
  setStatusTip(tr("Toggle the chart grid. Right click mouse for options."));
  setToolTip(tr("Toggle the chart grid"));
  setCheckable(TRUE);

  QSettings settings;
  settings.beginGroup("main" + g_session);

  QString s = settings.value("grid_color", "#626262").toString();
  _color.setNamedColor(s);

  setChecked(settings.value("grid", 1).toInt());

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void GridAction::changed (bool status)
{
  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("grid", status);
  settings.sync();

  emit signalChanged(status);
}

void GridAction::colorDialog ()
{
  QColorDialog *dialog = new QColorDialog(_color, this);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Grid Color"));
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(setColor(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}
  
void GridAction::setColor (QColor c)
{
  if (! c.isValid())
    return;

  _color = c;

  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("grid_color", _color.name());
  settings.sync();

  emit signalColorChanged(_color);
}

void GridAction::contextMenuEvent (QContextMenuEvent *)
{
  colorDialog();
}
