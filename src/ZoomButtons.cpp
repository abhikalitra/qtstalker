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

#include "ZoomButtons.h"
#include "Globals.h"
#include "../pics/zoomin.xpm"
#include "../pics/zoomout.xpm"

#include <QDebug>
#include <QString>
#include <QIcon>
#include <QSettings>

ZoomButtons::ZoomButtons (QToolBar *tb) : QObject (tb)
{
  QSettings settings(g_settingsFile);
  _pixelSpace = settings.value("pixelspace", 6).toInt();
  
  createButtons(tb);
}

void ZoomButtons::createButtons (QToolBar *tb)
{
  // zoom in button
  _zoomInButton = new QToolButton;
  _zoomInButton->setIcon(QIcon(zoomin_xpm));
  _zoomInButton->setToolTip(QString(tr("Zoom In")));
  _zoomInButton->setStatusTip(QString(tr("Zoom In")));
  _zoomInButton->setCheckable(FALSE);
  connect(_zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));
  tb->addWidget(_zoomInButton);
  
  // zoom out button
  _zoomOutButton = new QToolButton;
  _zoomOutButton->setIcon(QIcon(zoomout_xpm));
  _zoomOutButton->setToolTip(QString(tr("Zoom Out")));
  _zoomOutButton->setStatusTip(QString(tr("Zoom Out")));
  _zoomOutButton->setCheckable(FALSE);
  connect(_zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));
  tb->addWidget(_zoomOutButton);

  // PS1 button
  _ps1Button = new PixelSpaceButton("ps_button_1");
  connect(_ps1Button, SIGNAL(signalPixelSpaceChanged(int)), this, SLOT(psButtonClicked(int)));
  tb->addWidget(_ps1Button);

  // PS2 button
  _ps2Button = new PixelSpaceButton("ps_button_2");
  connect(_ps2Button, SIGNAL(signalPixelSpaceChanged(int)), this, SLOT(psButtonClicked(int)));
  tb->addWidget(_ps2Button);
}

void ZoomButtons::zoomIn ()
{
  _pixelSpace++;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace); 
}

void ZoomButtons::zoomOut ()
{
  _pixelSpace--;
  if (_pixelSpace < 6)
    _pixelSpace = 6;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace); 
}

void ZoomButtons::addZoom (int index, int ps)
{
  _pixelSpace = ps;
  savePixelSpace();
  emit signalZoom(_pixelSpace, index); 
}

void ZoomButtons::psButtonClicked (int ps)
{
  _pixelSpace = ps;
  savePixelSpace();
  emit signalPixelSpace(_pixelSpace);
}

int ZoomButtons::getPixelSpace ()
{
  return _pixelSpace;
}

void ZoomButtons::savePixelSpace ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("pixelspace", _pixelSpace);
  settings.sync();
}
