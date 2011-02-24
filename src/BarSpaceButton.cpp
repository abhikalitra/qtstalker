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

#include "BarSpaceButton.h"
#include "Globals.h"
#include "../pics/zoomin.xpm"
#include "../pics/zoomout.xpm"
#include "../pics/bar_spacing.xpm"

#include <QDebug>
#include <QString>
#include <QIcon>
#include <QSettings>

BarSpaceButton::BarSpaceButton ()
{
  QSettings settings(g_localSettings);
  _pixelSpace = settings.value("pixelspace", 8).toInt();
  
  createMenu();
  
  setPopupMode(QToolButton::InstantPopup);
  setToolTip(tr("Bar Spacing"));
  setStatusTip(tr("Bar Spacing"));
  setIcon(QIcon(bar_spacing_xpm));
}

void BarSpaceButton::createMenu ()
{
  _menu = new QMenu(this);
  _menu->setTitle(tr("Bar Spacing"));
//  connect(_menu, SIGNAL(triggered(QAction *)), this, SLOT(lengthChanged(QAction *)));
  setMenu(_menu);

  // zoom in
  _zoomIn = _menu->addAction(tr("Zoom In"));
  _zoomIn->setCheckable(FALSE);
  _zoomIn->setIcon(QIcon(zoomin_xpm));
  _zoomIn->setToolTip(QString(tr("Zoom In")));
  _zoomIn->setStatusTip(QString(tr("Zoom In")));
  connect(_zoomIn, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));

  // zoom out
  _zoomOut = _menu->addAction(tr("Zoom Out"));
  _zoomOut->setCheckable(FALSE);
  _zoomOut->setIcon(QIcon(zoomout_xpm));
  _zoomOut->setToolTip(QString(tr("Zoom Out")));
  _zoomOut->setStatusTip(QString(tr("Zoom Out")));
  connect(_zoomOut, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));

  _menu->addSeparator();

  // PS1 
  _ps1 = _menu->addAction("6");
  connect(_ps1, SIGNAL(triggered(bool)), this, SLOT(ps1Selected()));

  // PS2 button
  _ps2 = _menu->addAction("8");
  connect(_ps2, SIGNAL(triggered(bool)), this, SLOT(ps2Selected()));
}

void BarSpaceButton::zoomIn ()
{
  _pixelSpace++;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace); 
}

void BarSpaceButton::zoomOut ()
{
  _pixelSpace--;
  if (_pixelSpace < 6)
    _pixelSpace = 6;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace); 
}

void BarSpaceButton::addZoom (int index, int ps)
{
  _pixelSpace = ps;
  savePixelSpace();
  emit signalZoom(_pixelSpace, index); 
}

void BarSpaceButton::ps1Selected ()
{
  _pixelSpace = 6;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace);
}

void BarSpaceButton::ps2Selected ()
{
  _pixelSpace = 8;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace);
}

int BarSpaceButton::getPixelSpace ()
{
  return _pixelSpace;
}

void BarSpaceButton::savePixelSpace ()
{
  QSettings settings(g_localSettings);
  settings.setValue("pixelspace", _pixelSpace);
  settings.sync();
}
