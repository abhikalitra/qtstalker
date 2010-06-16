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
#include "Config.h"
#include "../pics/grid.xpm"

#include <QColorDialog>

GridAction::GridAction ()
{
  setIcon(QIcon(gridicon));
  setText(tr("Chart &Grid"));
  setStatusTip(tr("Toggle the chart grid. Right click mouse for options."));
  setToolTip(tr("Toggle the chart grid"));
  setCheckable(TRUE);

  Config config;
  QColor c;
  config.getData(Config::GridColor, c);
  if (! c.isValid())
  {
    c = QColor("#626262");
    config.setData(Config::GridColor, c);
  }

  QString s;
  config.getData(Config::Grid, s);
  if (! s.isEmpty())
    setChecked(s.toInt());
  else
  {
    config.setData(Config::Grid, 1);
    setChecked(TRUE);
  }

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void GridAction::changed (bool status)
{
  Config config;
  config.setData(Config::Grid, status);

  emit signalChanged(status);
}

void GridAction::colorDialog ()
{
  Config config;
  QColor oldColor;
  config.getData(Config::GridColor, oldColor);

  QColor newColor = QColorDialog::getColor(oldColor, this, tr("Select Grid Color"), 0);
  if (! newColor.isValid())
    return;

  if (oldColor != newColor)
  {
    config.setData(Config::GridColor, newColor);
    emit signalColorChanged(newColor);
  }
}

void GridAction::contextMenuEvent (QContextMenuEvent *)
{
  colorDialog();
}


