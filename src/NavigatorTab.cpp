/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "NavigatorTab.h"
#include <qcursor.h>
#include <qsettings.h>

NavigatorTabBar::NavigatorTabBar (QWidget *w) : QTabBar (w)
{
  menu = new QPopupMenu;
  
  positionMenu = new QPopupMenu();
  int id = positionMenu->insertItem(tr("Left"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(id, 0);
  id = positionMenu->insertItem(tr("Right"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(id, 1);
  menu->insertItem (tr("Navigator Position"), positionMenu);
}

NavigatorTabBar::~NavigatorTabBar ()
{
  delete menu;
}

void NavigatorTabBar::contextMenuEvent (QContextMenuEvent *event)
{
  event->accept();
  menu->exec(QCursor::pos());
}

void NavigatorTabBar::togglePosition (int pos)
{
  emit signalPositionChanged(pos);
}

//*****************************************************************
//*****************************************************************
//*****************************************************************

NavigatorTab::NavigatorTab (QWidget *w) : QTabWidget (w)
{
  NavigatorTabBar *bar = new NavigatorTabBar(this);
  QObject::connect(bar, SIGNAL(signalPositionChanged(int)), this, SLOT(togglePosition(int)));
  setTabBar(bar);
  loadSettings();
}

NavigatorTab::~NavigatorTab ()
{
}

void NavigatorTab::togglePosition (int pos)
{
  position = pos;
  emit signalPositionChanged(pos);
  saveSettings();
}

void NavigatorTab::saveSettings()
{
  QSettings settings;
  settings.writeEntry("/Qtstalker/NavigatorPosition", position);
}

void NavigatorTab::loadSettings() 
{
  QSettings settings;
  position = settings.readNumEntry("/Qtstalker/NavigatorPosition", 0);
}

int NavigatorTab::getPosition () 
{
  return position;
}

