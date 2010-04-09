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
#include <QLayout>
#include <QIcon>

ZoomButtons::ZoomButtons (QToolBar *tb) : QObject (tb)
{
  zoomPos = -1;
  
  createButtons(tb);
}

void ZoomButtons::createButtons (QToolBar *tb)
{
  Config config;
  int ti = config.getInt(Config::Pixelspace);
  
  Setting set;
  set.setData(0, 0); // save index, 0 for now
  set.setData(1, ti); // save pixelspace
  zoomList.append(set);
  zoomPos = 0;
  
  // zoom in button
  zoomInButton = new QToolButton;
  zoomInButton->setIcon(QIcon(zoomin_xpm));
  zoomInButton->setToolTip(QString(tr("Zoom In")));
  zoomInButton->setCheckable(FALSE);
  zoomInButton->setEnabled(FALSE);
  connect(zoomInButton, SIGNAL(clicked()), this, SLOT(zoomIn()));
  tb->addWidget(zoomInButton);
  
  // zoom out button
  zoomOutButton = new QToolButton;
  zoomOutButton->setIcon(QIcon(zoomout_xpm));
  zoomOutButton->setToolTip(QString(tr("Zoom Out")));
  zoomOutButton->setCheckable(FALSE);
  zoomOutButton->setEnabled(FALSE);
  connect(zoomOutButton, SIGNAL(clicked()), this, SLOT(zoomOut()));
  tb->addWidget(zoomOutButton);

  // PS1 button
  QString s;
  config.getData(Config::PSButton1, s);

  ps1Button = new QToolButton;
  ps1Button->setToolTip(QString(tr("Zoom Out")));
  ps1Button->setCheckable(FALSE);
  ps1Button->setToolTip(tr("Set Bar Spacing to ") + s);
  ps1Button->setText(s);
  connect(ps1Button, SIGNAL(clicked()), this, SLOT(ps1ButtonClicked()));
  tb->addWidget(ps1Button);

  // PS2 button
  config.getData(Config::PSButton2, s);

  ps2Button = new QToolButton;
  ps2Button->setToolTip(QString(tr("Zoom Out")));
  ps2Button->setCheckable(FALSE);
  ps2Button->setToolTip(tr("Set Bar Spacing to ") + s);
  ps2Button->setText(s);
  connect(ps2Button, SIGNAL(clicked()), this, SLOT(ps2ButtonClicked()));
  tb->addWidget(ps2Button);
}

void ZoomButtons::zoomIn ()
{
  zoomPos++;
  if (zoomPos == zoomList.count() - 1)
  {
    zoomInButton->setEnabled(FALSE);
    zoomOutButton->setEnabled(TRUE);
  }

  Setting set = zoomList[zoomPos];
  emit signalZoom(set.getInt(1), set.getInt(0)); 
}

void ZoomButtons::zoomOut ()
{
  zoomPos--;
  if (zoomPos == 0)
  {
    zoomInButton->setEnabled(TRUE);
    zoomOutButton->setEnabled(FALSE);
  }

  Setting set = zoomList[zoomPos];
  emit signalZoom(set.getInt(1), set.getInt(0)); 
}

void ZoomButtons::addZoom (int index, int pixelSpace)
{
  Setting set;
  set.setData(0, index);
  set.setData(1, pixelSpace);
  zoomList.append(set);

  zoomPos++;

  zoomOutButton->setEnabled(TRUE);

  emit signalZoom(pixelSpace, index); 
}

void ZoomButtons::resetZoom ()
{
  // clear the zoomList, leave first item as starting base
  int loop;
  for (loop = 1; loop < zoomList.count(); loop++)
    zoomList.removeAt(loop);

  zoomPos = 0;

  zoomInButton->setEnabled(FALSE);
  zoomOutButton->setEnabled(FALSE);
}

void ZoomButtons::ps1ButtonClicked ()
{
  Config config;
  int pixelSpace = config.getInt(Config::PSButton1);
  psButtonClicked(pixelSpace);
}

void ZoomButtons::ps2ButtonClicked ()
{
  Config config;
  int pixelSpace = config.getInt(Config::PSButton2);
  psButtonClicked(pixelSpace);
}

void ZoomButtons::psButtonClicked (int pixelSpace)
{
  resetZoom();
  
  Setting set = zoomList[0];
  set.setData(1, pixelSpace);
  zoomList[0] = set;

  zoomPos = 0;
  if (zoomList.count() > 1)
    zoomInButton->setEnabled(TRUE);
  zoomOutButton->setEnabled(FALSE);

  Config config;
  config.setData((int) Config::Pixelspace, pixelSpace); // save base zoom amount

  emit signalPixelSpace(pixelSpace);
}

int ZoomButtons::getPixelSpace ()
{
  Setting set = zoomList[zoomPos];
  return set.getInt(1);
}

void ZoomButtons::setSetting (int pos, int index, int pixelSpace)
{
  Setting set = zoomList[pos];
  set.setData(0, index);
  set.setData(1, pixelSpace);
}

void ZoomButtons::getSetting (int pos, int &index, int &pixelSpace)
{
  Setting set = zoomList[pos];
  index = set.getInt(0);
  pixelSpace = set.getInt(1);
}

