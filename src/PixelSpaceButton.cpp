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

#include "PixelSpaceButton.h"
#include "Config.h"

#include <QDebug>
#include <QString>
#include <QInputDialog>

PixelSpaceButton::PixelSpaceButton (int d)
{
  _key = d;
  
  Config config;
  _pixelSpace = config.getInt((Config::Parm) _key);
  QString s = QString::number(_pixelSpace);

  setCheckable(FALSE);
  setToolTip(tr("Set Bar Spacing to ") + s);
  setText(s);

  connect(this, SIGNAL(clicked()), this, SLOT(buttonClicked()));
}

void PixelSpaceButton::contextMenuEvent (QContextMenuEvent *)
{
  dialog();
}

void PixelSpaceButton::buttonClicked ()
{
  emit signalPixelSpaceChanged(_pixelSpace);
}

void PixelSpaceButton::setPixelSpace (int d)
{
  _pixelSpace = d;
  
  QString s = QString::number(_pixelSpace);
  setToolTip(tr("Set Bar Spacing to ") + s);
  setText(s);

  Config config;
  config.setData((Config::Parm) _key, _pixelSpace);
}

void PixelSpaceButton::dialog ()
{
  bool ok;
  int t = QInputDialog::getInt(this,
                               tr("Set pixel spacing button 1"),
                               tr("Enter pixel spacing"),
                               _pixelSpace,
                               6,
                               99,
                               1,
                               &ok,
                               0);

  if (! ok)
    return;

  setPixelSpace(t);
}
