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

#include "IndicatorTab.h"
#include <qcursor.h>
#include <qsettings.h>

IndicatorTabBar::IndicatorTabBar (QWidget *w) : QTabBar (w)
{
  menu = new QPopupMenu;
  
  positionMenu = new QPopupMenu();
  int id = positionMenu->insertItem(tr("Top"), this, SLOT(toggleTabPosition(int)));
  positionMenu->setItemParameter(id, 0);
  id = positionMenu->insertItem(tr("Bottom"), this, SLOT(toggleTabPosition(int)));
  positionMenu->setItemParameter(id, 1);
  menu->insertItem (tr("Tab Position"), positionMenu);
}

IndicatorTabBar::~IndicatorTabBar ()
{
  delete menu;
}

void IndicatorTabBar::contextMenuEvent (QContextMenuEvent *event)
{
  event->accept();
  menu->exec(QCursor::pos());
}

void IndicatorTabBar::toggleTabPosition (int pos)
{
  switch (pos)
  {
    case 1:
      emit signalPositionChanged(QTabWidget::Bottom);
      break;
    default:
      emit signalPositionChanged(QTabWidget::Top);
      break;
  }
}

//*****************************************************************
//*****************************************************************
//*****************************************************************

IndicatorTab::IndicatorTab (QWidget *w) : QTabWidget (w)
{
  IndicatorTabBar *bar = new IndicatorTabBar(this);
  QObject::connect(bar, SIGNAL(signalPositionChanged(QTabWidget::TabPosition)),
                   this, SLOT(toggleTabPosition(QTabWidget::TabPosition)));
  setTabBar(bar);
  loadSettings();
}

IndicatorTab::~IndicatorTab ()
{
}

void IndicatorTab::toggleTabPosition (QTabWidget::TabPosition pos)
{
  setTabPosition(pos);
  saveSettings();
}

void IndicatorTab::saveSettings()
{
  QSettings settings;
  if (tabPosition() == QTabWidget::Top)
    settings.writeEntry("/Qtstalker/IndicatorTabs", TRUE);
  else
    settings.writeEntry("/Qtstalker/IndicatorTabs", FALSE);
}

void IndicatorTab::loadSettings() 
{
  QSettings settings;
  bool pos = settings.readBoolEntry("/Qtstalker/IndicatorTabs", TRUE);
  if (pos == TRUE)
    setTabPosition(QTabWidget::Top);
  else
    setTabPosition(QTabWidget::Bottom);
}

