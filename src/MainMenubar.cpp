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

#include "MainMenubar.h"
#include <qaccel.h>
#include "../pics/done.xpm"
#include "../pics/grid.xpm"
#include "../pics/datawindow.xpm"
#include "../pics/indicator.xpm"
#include "../pics/quotes.xpm"
#include "../pics/configure.xpm"
#include "../pics/scaletoscreen.xpm"
#include "../pics/nav.xpm"
#include "../pics/loggrid.xpm"
#include "../pics/hidechart.xpm"
#include "../pics/date.xpm"
#include "../pics/co.xpm"
#include "../pics/help.xpm"
#include "../pics/qtstalker.xpm"


MainMenubar::MainMenubar (QMainWindow *mw) : QMenuBar (mw, "mainMenubar")
{
  macroFlag = FALSE;
  tmacro = 0;
  keyFlag = FALSE;
  actions.setAutoDelete(FALSE);
  
  QPixmap icon(finished);
  QAction *action  = new QAction(this, "actionExit");
  action->setMenuText(tr("E&xit"));
  action->setIconSet(icon);
  action->setAccel(CTRL+Key_1);
  action->setStatusTip(tr("Quit Qtstalker (Ctrl+1)"));
  action->setToolTip(tr("Quit Qtstalker (Ctrl+1)"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalExit()));
  actions.replace(Exit, action);
  
  icon = indicator;
  action = new QAction(this, "actionNewIndicator");
  action->setMenuText(tr("New &Indicator"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Add a new indicator to chart (Ctrl+2)"));
  action->setToolTip(tr("Add a new indicator to chart (Ctrl+2)"));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalNewIndicator()));
  actions.replace(NewIndicator, action);

  icon = configure;
  action = new QAction(this, "actionOptions");
  action->setMenuText(tr("Edit &Preferences"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Modify user preferences  (Ctrl+3)"));
  action->setToolTip(tr("Modify user preferences  (Ctrl+3)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotOptions()));
  actions.replace(Options, action);

  icon = gridicon;
  QString s = config.getData(Config::Grid);
  action = new QAction(this, "actionGrid");
  action->setMenuText(tr("Chart &Grid"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setToolTip(tr("Toggle the chart grid  (Ctrl+4)"));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalGrid(bool)));
  actions.replace(Grid, action);

  icon = quotes;
  action = new QAction(this, "actionQuote");
  action->setMenuText(tr("&Quotes"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Download quotes from internet  (Alt+2)"));
  action->setToolTip(tr("Download quotes from internet  (Alt+2)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotQuotes()));
  actions.replace(Quotes, action);

  icon = datawindow;
  action = new QAction(this, "actionDataWindow");
  action->setMenuText(tr("&Data Window"));
  action->setIconSet(icon);
  action->setAccel(ALT+Key_1);
  action->setStatusTip(tr("Show the data window (Alt+1)"));
  action->setToolTip(tr("Show the data window (Alt+1)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotDataWindow()));
  actions.replace(DataWindow, action);

  icon = qtstalker;
  action = new QAction(this, "actionAbout");
  action->setMenuText(tr("&About"));
  action->setIconSet(icon);
  action->setStatusTip(tr("About Qtstalker."));
  action->setToolTip(tr("About Qtstalker."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotAbout()));
  actions.replace(About, action);

  icon = scaletoscreen;
  s = config.getData(Config::ScaleToScreen);
  action = new QAction(this, "actionScale");
  action->setMenuText(tr("&Scale To Screen"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setToolTip(tr("Scale chart to current screen data (Ctrl+5)"));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actions.replace(ScaleToScreen, action);

  icon = nav;
  action = new QAction(this, "actionPanel");
  action->setMenuText(tr("Side Pa&nel"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setToolTip(tr("Toggle the side panel area from view (Ctrl+7)"));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), mw, SLOT(slotHideNav(bool)));
  actions.replace(SidePanel, action);

  icon = loggridicon;
  s = config.getData(Config::LogScale);
  action = new QAction(this, "actionLog");
  action->setMenuText(tr("&Log Scaling"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle log scaling (Ctrl+6)"));
  action->setToolTip(tr("Toggle log scaling (Ctrl+6)"));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalLog(bool)));
  actions.replace(Log, action);

  icon = hidechart;
  action = new QAction(this, "actionHideMain");
  action->setMenuText(tr("Hide &Main Plot"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Hide the main plot (Ctrl+8)"));
  action->setToolTip(tr("Hide the main plot (Ctrl+8)"));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalHideMain(bool)));
  actions.replace(HideMain, action);

  icon = date;
  action = new QAction(this, "actionDate");
  action->setMenuText(tr("Toggle Indicator Date"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle indicator date (Ctrl+9)"));
  action->setToolTip(tr("Toggle indicator date (Ctrl+9)"));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalPlotDate(bool)));
  actions.replace(IndicatorDate, action);

  icon = co;
  s = config.getData(Config::DrawMode);
  action = new QAction(this, "actionDraw");
  action->setMenuText(tr("Toggle Dra&w Mode"));
  action->setIconSet(icon);
  action->setStatusTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setToolTip(tr("Toggle drawing mode (Ctrl+0)"));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalDraw(bool)));
  actions.replace(DrawMode, action);
  
  icon = help;
  action = new QAction(this, "actionHelp");
  action->setMenuText(tr("&Help"));
  action->setIconSet(icon);
  action->setAccel(ALT+Key_3);
  action->setStatusTip(tr("Display Help Dialog (Alt+3)"));
  action->setToolTip(tr("Display Help Dialog (Alt+3)"));
  connect(action, SIGNAL(activated()), mw, SLOT(slotHelp()));
  actions.replace(Help, action);
  
  QAccel *a = new QAccel(mw);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_2, NewIndicator);
  a->insertItem(CTRL+Key_3, Options);
  a->insertItem(CTRL+Key_4, Grid);
  a->insertItem(CTRL+Key_5, ScaleToScreen);
  a->insertItem(CTRL+Key_6, Log);
  a->insertItem(CTRL+Key_7, SidePanel);
  a->insertItem(CTRL+Key_8, HideMain);
  a->insertItem(CTRL+Key_9, IndicatorDate);
  a->insertItem(CTRL+Key_0, DrawMode);
  a->insertItem(ALT+Key_2, Quotes);
  
  a->insertItem(CTRL+Key_Escape, 8);
  a->connectItem(8, mw, SLOT(slotStopMacro()));
 
  a->insertItem(Key_F1, Macro1);
  a->insertItem(Key_F2, Macro2);
  a->insertItem(Key_F3, Macro3);
  a->insertItem(Key_F4, Macro4);
  a->insertItem(Key_F5, Macro5);
  a->insertItem(Key_F6, Macro6);
  a->insertItem(Key_F7, Macro7);
  a->insertItem(Key_F8, Macro8);
  a->insertItem(Key_F9, Macro9);
  a->insertItem(Key_F10, Macro10);
  a->insertItem(Key_F11, Macro11);
  a->insertItem(Key_F12, Macro12);
  
  createMenus();
  
  s = config.getData(Config::Menubar);
  if (! s.toInt())
    hide();
}

MainMenubar::~MainMenubar ()
{
}

void MainMenubar::createMenus ()
{
  fileMenu = new QPopupMenu();
  actions[Exit]->addTo(fileMenu);

  editMenu = new QPopupMenu();
  actions[NewIndicator]->addTo(editMenu);
  actions[Options]->addTo(editMenu);

  viewMenu = new QPopupMenu();
  viewMenu->setCheckable(true);
  actions[Grid]->addTo(viewMenu);
  actions[ScaleToScreen]->addTo(viewMenu);
  actions[Log]->addTo(viewMenu);
  actions[SidePanel]->addTo(viewMenu);
  actions[HideMain]->addTo(viewMenu);
  actions[IndicatorDate]->addTo(viewMenu);
  actions[DrawMode]->addTo(viewMenu);

  toolMenu = new QPopupMenu();
  actions[DataWindow]->addTo(toolMenu);
  actions[Quotes]->addTo(toolMenu);

  helpMenu = new QPopupMenu();
  actions[About]->addTo(helpMenu);
  actions[Help]->addTo(helpMenu);
  
  insertItem(tr("&File"), fileMenu);
  insertItem(tr("&Edit"), editMenu);
  insertItem(tr("&View"), viewMenu);
  insertItem(tr("&Tools"), toolMenu);
  insertSeparator();
  insertItem(tr("&Help"), helpMenu);
}

QAction * MainMenubar::getAction (int d)
{
  return actions[d];
}

bool MainMenubar::getStatus (int d)
{
  return actions[d]->isOn();
}

void MainMenubar::setStatus (int d, bool f)
{
  actions[d]->setOn(f);
}

void MainMenubar::saveSettings ()
{
  config.setData(Config::DrawMode, QString::number(getStatus(DrawMode)));
}

void MainMenubar::setKeyFlag (bool d)
{
  keyFlag = d;
}

void MainMenubar::slotAccel (int id)
{
  switch (id)
  {
    case NewIndicator:
      emit signalNewIndicator();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_2, 0, QString());
      break;
    case Options:
      emit signalOptions();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_3, 0, QString());
      break;
    case Grid:
      getAction(Grid)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_4, 0, QString());
      break;
    case Quotes:
      emit signalQuotes();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, AltButton, Key_2, 0, QString());
      break;
    case ScaleToScreen:
      getAction(ScaleToScreen)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_5, 0, QString());
      break;
    case SidePanel:
      getAction(SidePanel)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_7, 0, QString());
      break;
    case Log:
      getAction(Log)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_6, 0, QString());
      break;
    case HideMain:
      getAction(HideMain)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_8, 0, QString());
      break;
    case IndicatorDate:
      getAction(IndicatorDate)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_9, 0, QString());
      break;
    case DrawMode:
      getAction(DrawMode)->toggle();
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, ControlButton, Key_0, 0, QString());
      break;
    case Macro1:
      emit signalRunMacro(config.getData(Config::Macro1));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F1, 0, QString());
      break;
    case Macro2:
      emit signalRunMacro(config.getData(Config::Macro2));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F2, 0, QString());
      break;
    case Macro3:
      emit signalRunMacro(config.getData(Config::Macro3));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F3, 0, QString());
      break;
    case Macro4:
      emit signalRunMacro(config.getData(Config::Macro4));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F4, 0, QString());
      break;
    case Macro5:
      emit signalRunMacro(config.getData(Config::Macro5));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F5, 0, QString());
      break;
    case Macro6:
      emit signalRunMacro(config.getData(Config::Macro6));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F6, 0, QString());
      break;
    case Macro7:
      emit signalRunMacro(config.getData(Config::Macro7));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F7, 0, QString());
      break;
    case Macro8:
      emit signalRunMacro(config.getData(Config::Macro8));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F8, 0, QString());
      break;
    case Macro9:
      emit signalRunMacro(config.getData(Config::Macro9));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F9, 0, QString());
      break;
    case Macro10:
      emit signalRunMacro(config.getData(Config::Macro10));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F10, 0, QString());
      break;
    case Macro11:
      emit signalRunMacro(config.getData(Config::Macro11));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F11, 0, QString());
      break;
    case Macro12:
      emit signalRunMacro(config.getData(Config::Macro12));
      if (keyFlag)
        emit signalKeyPressed (Macro::Menubar, 0, Key_F12, 0, QString());
      break;
    default:
      break;
  }
}

void MainMenubar::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_2:
	slotAccel(NewIndicator);
        break;
      case Qt::Key_3:
	slotAccel(Options);
        break;
      case Qt::Key_4:
	slotAccel(Grid);
        break;
      case Qt::Key_5:
	slotAccel(ScaleToScreen);
        break;
      case Qt::Key_6:
	slotAccel(Log);
        break;
      case Qt::Key_7:
	slotAccel(SidePanel);
        break;
      case Qt::Key_8:
	slotAccel(HideMain);
        break;
      case Qt::Key_9:
	slotAccel(IndicatorDate);
        break;
      case Qt::Key_0:
	slotAccel(DrawMode);
        break;
      default:
        break;
    }
  }
  else
  {
    if (key->state() == Qt::AltButton)
    {
      switch (key->key())
      {
        case Qt::Key_2:
	  slotAccel(Quotes);
          break;
        default:
          break;
      }
    }
    else
    {
      switch (key->key())
      {
        case Qt::Key_F1:
	  slotAccel(Macro1);
          break;
        case Qt::Key_F2:
	  slotAccel(Macro2);
          break;
        case Qt::Key_F3:
	  slotAccel(Macro3);
          break;
        case Qt::Key_F4:
	  slotAccel(Macro4);
          break;
        case Qt::Key_F5:
	  slotAccel(Macro5);
          break;
        case Qt::Key_F6:
	  slotAccel(Macro6);
          break;
        case Qt::Key_F7:
	  slotAccel(Macro7);
          break;
        case Qt::Key_F8:
	  slotAccel(Macro8);
          break;
        case Qt::Key_F9:
	  slotAccel(Macro9);
          break;
        case Qt::Key_F10:
	  slotAccel(Macro10);
          break;
        case Qt::Key_F11:
	  slotAccel(Macro11);
          break;
        case Qt::Key_F12:
	  slotAccel(Macro12);
          break;
        default:
          break;
      }
    }
  }
}

void MainMenubar::runMacro (Macro *d)
{
  tmacro = d;
  macroFlag = TRUE;
  
  while (tmacro->getZone(tmacro->getIndex()) == Macro::Menubar)
  {
    doKeyPress(tmacro->getKey(tmacro->getIndex()));
    
    tmacro->incIndex();
    if (tmacro->getIndex() >= tmacro->getCount())
      break;
  }
  
  macroFlag = FALSE;
}

