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
#include "ChartPage.h"
#include "IndicatorPage.h"
#include "ScannerPage.h"
#include "PortfolioPage.h"
#include "TestPage.h"
#include "GroupPage.h"
#include "MacroPage.h"
#include "indicator.xpm"
#include "dirclosed.xpm"
#include "plainitem.xpm"
#include "portfolio.xpm"
#include "test.xpm"
#include "scanner.xpm"
#include "macro.xpm"
#include <qcursor.h>
#include <qsettings.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

NavigatorTab::NavigatorTab (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setSpacing(0);
  vbox->setMargin(0);

  QHBoxLayout *hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(1);
  hbox->setMargin(2);
  
  bg = new QButtonGroup(this);
  QObject::connect(bg, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
  bg->setExclusive(TRUE);
  bg->hide();
  
  QPushButton *button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Charts"));
  button->setPixmap(plainitem);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 0);
  bg->setButton(0);
  
  button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Groups"));
  button->setPixmap(dirclosed);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 1);

  button = new QPushButton(this);
  QToolTip::add(button, tr("Main Chart Indicators"));
  button->setPixmap(indicator);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 2);
  
  button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Portfolios"));
  button->setPixmap(portfolio);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 3);

  hbox->addStretch(1);
    
  hbox = new QHBoxLayout(vbox);
  hbox->setSpacing(1);
  hbox->setMargin(2);

  button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Backtesting"));
  button->setPixmap(test);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 4);
  
  button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Scanner"));
  button->setPixmap(scanner);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 5);
  
  button = new QPushButton(this);
  QToolTip::add(button, tr("Workwith Macro"));
  button->setPixmap(macro);
  button->setMaximumWidth(25);
  button->setMaximumHeight(25);
  button->setToggleButton(TRUE);
  hbox->addWidget(button);
  bg->insert(button, 6);
  
  hbox->addStretch(1);
  
  stack = new QWidgetStack(this);
  vbox->addWidget(stack);

  menu = new QPopupMenu;
  
  positionMenu = new QPopupMenu();
  int id = positionMenu->insertItem(tr("Left"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(id, 0);
  id = positionMenu->insertItem(tr("Right"), this, SLOT(togglePosition(int)));
  positionMenu->setItemParameter(id, 1);
  menu->insertItem (tr("Navigator Position"), positionMenu);

  loadSettings();
}

NavigatorTab::~NavigatorTab ()
{
  delete menu;
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

void NavigatorTab::contextMenuEvent (QContextMenuEvent *event)
{
  event->accept();
  menu->exec(QCursor::pos());
}

void NavigatorTab::addWidget (QWidget *w, int id) 
{
  stack->addWidget(w, id);
}

void NavigatorTab::buttonPressed (int id) 
{
  stack->raiseWidget(id);
  QWidget *w = 0;
  
  switch (id)
  {
    case 0:
      w = (ChartPage *) stack->widget(id);
      break;
    case 1:
      w = (GroupPage *) stack->widget(id);
      break;
    case 2:
      w = (IndicatorPage *) stack->widget(id);
      break;
    case 3:
      w = (PortfolioPage *) stack->widget(id);
      break;
    case 4:
      w = (TestPage *) stack->widget(id);
      break;
    case 5:
      w = (ScannerPage *) stack->widget(id);
      break;
    case 6:
      w = (MacroPage *) stack->widget(id);
      break;
    default:
      break;
  }

  if (w)
    w->setFocus();
}

void NavigatorTab::pressButton (int id) 
{
  if (bg->selectedId() == id)
    return;
    
  bg->find(id)->toggle();
  buttonPressed(id);
}


