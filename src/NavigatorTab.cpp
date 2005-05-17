/*
 *  Qtstalker stock charter
 * 
 *  Copyright (C) 2001-2005 Stefan S. Stratigakos
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
#include "../pics/indicator.xpm"
#include "../pics/dirclosed.xpm"
#include "../pics/plainitem.xpm"
#include "../pics/portfolio.xpm"
#include "../pics/test.xpm"
#include "../pics/scanner.xpm"
#include "../pics/macro.xpm"
#include <qcursor.h>
#include <qsettings.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qaccel.h>

NavigatorTab::NavigatorTab (QWidget *w, QMainWindow *mw) : QWidget (w)
{
  macroFlag = FALSE;
  tmacro = 0;
  keyFlag = FALSE;

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
  
  QAccel *a = new QAccel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_C, ChartPanelFocus);
  a->insertItem(CTRL+Key_G, GroupPanelFocus);
  a->insertItem(CTRL+Key_I, IndicatorPanelFocus);
  a->insertItem(CTRL+Key_P, PortfolioPanelFocus);
  a->insertItem(CTRL+Key_T, TestPanelFocus);
  a->insertItem(CTRL+Key_S, ScannerPanelFocus);
  a->insertItem(CTRL+Key_M, MacroPanelFocus);

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
  if (bg->id(bg->selected()) == id)
  {
    buttonPressed(id);
    return;
  }
    
  bg->find(id)->toggle();
  buttonPressed(id);
}

void NavigatorTab::doKeyPress (QKeyEvent *key)
{
  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_C:
        key->accept();
	pressButton(0);
        break;
      case Qt::Key_G:
        key->accept();
	pressButton(1);
        break;
      case Qt::Key_I:
        key->accept();
	pressButton(2);
        break;
      case Qt::Key_P:
        key->accept();
	pressButton(3);
        break;
      case Qt::Key_T:
        key->accept();
	pressButton(4);
        break;
      case Qt::Key_S:
        key->accept();
	pressButton(5);
        break;
      case Qt::Key_M:
        key->accept();
	pressButton(6);
        break;
      default:
        break;
    }
  }
  else
    key->ignore();
}

void NavigatorTab::runMacro (Macro *d)
{
  tmacro = d;
  macroFlag = TRUE;
  
  while (tmacro->getZone(tmacro->getIndex()) == Macro::SidePanel)
  {
    doKeyPress(tmacro->getKey(tmacro->getIndex()));
    
    tmacro->incIndex();
    if (tmacro->getIndex() >= tmacro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

void NavigatorTab::slotAccel (int id)
{
  switch (id)
  {
    case ChartPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_C, 0, QString());
      pressButton(id);
      break;
    case GroupPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_G, 0, QString());
      pressButton(id);
      break;
    case IndicatorPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_I, 0, QString());
      pressButton(id);
      break;
    case PortfolioPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_P, 0, QString());
      pressButton(id);
      break;
    case TestPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_T, 0, QString());
      pressButton(id);
      break;
    case ScannerPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_S, 0, QString());
      pressButton(id);
      break;
    case MacroPanelFocus:
      if (keyFlag)
        emit signalKeyPressed (Macro::SidePanel, ControlButton, Key_M, 0, QString());
      pressButton(id);
      break;
    default:
      break;
  }
}

void NavigatorTab::setKeyFlag (bool d)
{
  keyFlag = d;
}

