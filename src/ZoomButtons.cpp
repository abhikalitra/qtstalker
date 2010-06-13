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
#include "Config.h"

#include "../pics/zoomin.xpm"
#include "../pics/zoomout.xpm"

#include <QDebug>
#include <QString>
#include <QIcon>

ZoomButtons::ZoomButtons (QToolBar *tb) : QObject (tb)
{
  pixelSpace = 6;

  createButtons(tb);
}

void ZoomButtons::createButtons (QToolBar *tb)
{
  Config config;
  pixelSpace = config.getInt(Config::Pixelspace);

  // zoom in button
  zoomInButton = new QToolButton;
  zoomInButton->setIcon(QIcon(zoomin_xpm));
  zoomInButton->setToolTip(QString(tr("Zoom In")));
  zoomInButton->setCheckable(FALSE);
  connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));
  tb->addWidget(zoomInButton);
  
  // zoom out button
  zoomOutButton = new QToolButton;
  zoomOutButton->setIcon(QIcon(zoomout_xpm));
  zoomOutButton->setToolTip(QString(tr("Zoom Out")));
  zoomOutButton->setCheckable(FALSE);
  connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));
  tb->addWidget(zoomOutButton);

  // PS1 button
  ps1Button = new PixelSpaceButton((int) Config::PSButton1);
  connect(ps1Button, SIGNAL(signalPixelSpaceChanged(int)), this, SLOT(psButtonClicked(int)));
  tb->addWidget(ps1Button);

  // PS2 button
  ps2Button = new PixelSpaceButton((int) Config::PSButton2);
  connect(ps2Button, SIGNAL(signalPixelSpaceChanged(int)), this, SLOT(psButtonClicked(int)));
  tb->addWidget(ps2Button);
}

void ZoomButtons::zoomIn ()
{
  pixelSpace++;
  emit signalPixelSpace(pixelSpace); 
}

void ZoomButtons::zoomOut ()
{
  pixelSpace--;
  if (pixelSpace < 6)
    pixelSpace = 6;
  emit signalPixelSpace(pixelSpace); 
}

void ZoomButtons::addZoom (int index, int ps)
{
  pixelSpace = ps;
  emit signalZoom(pixelSpace, index); 
}

void ZoomButtons::psButtonClicked (int ps)
{
  pixelSpace = ps;
  emit signalPixelSpace(pixelSpace);
}

int ZoomButtons::getPixelSpace ()
{
  return pixelSpace;
}

