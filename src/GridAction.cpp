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
#include "Globals.h"

#include <QColorDialog>

GridAction::GridAction ()
{
  setIcon(QIcon(gridicon));
  setText(tr("Chart &Grid"));
  setStatusTip(tr("Toggle the chart grid. Right click mouse for options."));
  setToolTip(tr("Toggle the chart grid"));
  setCheckable(TRUE);

  Config config;
  config.transaction();
  
  config.getData(Config::GridColor, _color);
  if (! _color.isValid())
  {
    _color = QColor("#626262");
    config.setData(Config::GridColor, _color);
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

  config.commit();

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void GridAction::changed (bool status)
{
  Config config;
  config.transaction();
  config.setData(Config::Grid, status);
  config.commit();

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

  Config config;
  config.transaction();
  config.setData(Config::GridColor, _color);
  config.commit();

  emit signalColorChanged(_color);
}

void GridAction::contextMenuEvent (QContextMenuEvent *)
{
  colorDialog();
}


