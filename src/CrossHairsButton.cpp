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
#include "Config.h"
#include "../pics/crosshair.xpm"
#include "Globals.h"

#include <QString>
#include <QDebug>
#include <QIcon>
#include <QColorDialog>

CrossHairsButton::CrossHairsButton ()
{
  setIcon(QIcon(crosshair_xpm));
  setText(tr("Cursor Crosshairs"));
  setStatusTip(tr("Toggle the cursor crosshairs. Right click mouse for options."));
  setToolTip(tr("Toggle the cursor crosshairs"));
  setCheckable(TRUE);

  Config config;
  QString s;
  config.getData(Config::CrossHairs, s);
  if (s.isEmpty())
  {
    setChecked(0);
    config.setData(Config::CrossHairs, 0);
  }
  else
    setChecked(s.toInt());

  config.getData(Config::CrossHairsColor, _color);
  if (! _color.isValid())
  {
    _color = QColor(Qt::white);
    config.setData(Config::CrossHairsColor, _color);
  }

  connect(this, SIGNAL(toggled(bool)), this, SLOT(changed(bool)));
}

void CrossHairsButton::changed (bool status)
{
  Config config;
  config.setData(Config::CrossHairs, status);

  emit signalChanged(status);
}

void CrossHairsButton::dialog ()
{
  QColorDialog *dialog = new QColorDialog(_color, this);
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Crosshairs Color"));
  connect(dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(setColor(QColor)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void CrossHairsButton::setColor (QColor c)
{
  if (! c.isValid())
    return;
  
  _color = c;

  Config config;
  config.setData(Config::CrossHairsColor, _color);
  emit signalColor(_color);
}

void CrossHairsButton::contextMenuEvent (QContextMenuEvent *)
{
  dialog();
}

