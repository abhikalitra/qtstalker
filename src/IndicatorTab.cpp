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

IndicatorTab::IndicatorTab (QWidget *w) : QTabWidget (w)
{
  menu = new QPopupMenu;
  
  positionMenu = new QPopupMenu();
  int id = positionMenu->insertItem(tr("Top"), this, SLOT(toggleTabPosition(int)));
  positionMenu->setItemParameter(id, 0);
  id = positionMenu->insertItem(tr("Bottom"), this, SLOT(toggleTabPosition(int)));
  positionMenu->setItemParameter(id, 1);
  menu->insertItem (tr("Tab Position"), positionMenu);
  
  loadSettings();
}

IndicatorTab::~IndicatorTab ()
{
  delete menu;
}

void IndicatorTab::mousePressEvent (QMouseEvent *event)
{
  if (event->button() != RightButton)
  {
    event->ignore();
    return;
  }

  menu->exec(QCursor::pos());
}

void IndicatorTab::toggleTabPosition (int pos)
{
  switch (pos)
  {
    case 1:
      setTabPosition(QTabWidget::Bottom);
      break;
    default:
      setTabPosition(QTabWidget::Top);
      break;
  }
      
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
