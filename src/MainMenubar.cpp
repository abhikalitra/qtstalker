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
#include "Config.h"
#include "done.xpm"
#include "grid.xpm"
#include "datawindow.xpm"
#include "indicator.xpm"
#include "quotes.xpm"
#include "configure.xpm"
#include "scaletoscreen.xpm"
#include "nav.xpm"
#include "loggrid.xpm"
#include "hidechart.xpm"
#include "date.xpm"
#include "co.xpm"
#include "help.xpm"
#include "qtstalker.xpm"

MainMenubar::MainMenubar (QMainWindow *mw) : QMenuBar (mw, "mainMenubar")
{
  actions.setAutoDelete(FALSE);
  Config config;
  
  QPixmap icon(finished);
//  QAction *action  = new QAction(tr("Exit"), icon, tr("E&xit"), CTRL+Key_X, this);
  QAction *action  = new QAction(icon, tr("E&xit"), 0, this, "actionExit");
  action->setStatusTip(tr("Quit Qtstalker."));
  connect(action, SIGNAL(activated()), this, SIGNAL(signalExit()));
  actions.replace(Exit, action);
  
  icon = indicator;
//  action = new QAction(tr("New Indicator..."), icon, tr("New &Indicator..."), CTRL+Key_I, this);
  action = new QAction(icon, tr("New &Indicator"), 0, this, "actionNewIndicator");
  action->setStatusTip(tr("Add a new indicator to chart."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotNewIndicator()));
  actions.replace(NewIndicator, action);

  icon = configure;
//  action = new QAction(tr("Edit Preferences..."), icon, tr("Edit &Preferences..."), CTRL+Key_P, this);
  action = new QAction(icon, tr("Edit &Preferences"), 0, this, "actionOptions");
  action->setStatusTip(tr("Modify user preferences."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotOptions()));
  actions.replace(Options, action);

  icon = gridicon;
  QString s = config.getData(Config::Grid);
//  action = new QAction(tr("Chart Grid"), icon, tr("Chart &Grid"), CTRL+Key_G, this, 0, true);
  action = new QAction(icon, tr("Chart &Grid"), 0, this, "actionGrid");
  action->setStatusTip(tr("Toggle the chart grid."));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalGrid(bool)));
  actions.replace(Grid, action);

  icon = quotes;
//  action = new QAction(tr("Quotes..."), icon, tr("&Quotes..."), CTRL+Key_Q, this);
  action = new QAction(icon, tr("&Quotes..."), 0, this, "actionQuote");
  action->setStatusTip(tr("Download quotes from internet."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotQuotes()));
  actions.replace(Quotes, action);

  icon = datawindow;
//  action = new QAction(tr("Data Window..."), icon, tr("&Data Window..."), CTRL+Key_D, this);
  action = new QAction(icon, tr("&Data Window..."), 0, this, "actionDataWindow");
  action->setStatusTip(tr("Show the data window."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotDataWindow()));
  actions.replace(DataWindow, action);

  icon = qtstalker;
//  action = new QAction(tr("About"), icon, tr("&About..."), CTRL+Key_A, this);
  action = new QAction(icon, tr("&About..."), 0, this, "actionAbout");
  action->setStatusTip(tr("About Qtstalker."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotAbout()));
  actions.replace(About, action);

  icon = scaletoscreen;
  s = config.getData(Config::ScaleToScreen);
//  action = new QAction(tr("Scale To Screen"), icon, tr("&Scale To Screen"), CTRL+Key_S, this, 0, true);
  action = new QAction(icon, tr("&Scale To Screen"), 0, this, "actionScale");
  action->setStatusTip(tr("Scale chart to current screen data."));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalScale(bool)));
  actions.replace(ScaleToScreen, action);

  icon = nav;
//  action = new QAction(tr("Side Panel"), icon, tr("Side Pa&nel"), CTRL+Key_N, this, 0, true);
  action = new QAction(icon, tr("Side Pa&nel"), 0, this, "actionPanel");
  action->setStatusTip(tr("Toggle the side panel area from view."));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), mw, SLOT(slotHideNav(bool)));
  actions.replace(SidePanel, action);

  icon = loggridicon;
  s = config.getData(Config::LogScale);
//  action = new QAction(tr("Log Scaling"), icon, tr("&Log Scaling"), CTRL+Key_L, this, 0, true);
  action = new QAction(icon, tr("&Log Scaling"), 0, this, "actionLog");
  action->setStatusTip(tr("Toggle log scaling."));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalLog(bool)));
  actions.replace(Log, action);

  icon = hidechart;
//  action = new QAction(tr("Hide Main Plot"), icon, tr("Hide &Main Plot"), CTRL+Key_M, this, 0, true);
  action = new QAction(icon, tr("Hide &Main Plot"), 0, this, "actionHideMain");
  action->setStatusTip(tr("Hide the main plot."));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalHideMain(bool)));
  actions.replace(HideMain, action);

  icon = date;
//  action = new QAction(tr("Toggle Indicator Date"), icon, tr("Toggle Indicator Date"), 0, this, 0, true);
  action = new QAction(icon, tr("Toggle Indicator Date"), 0, this, "actionDate");
  action->setStatusTip(tr("Toggle indicator date."));
  action->setToggleAction(TRUE);
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalPlotDate(bool)));
  actions.replace(IndicatorDate, action);

  icon = co;
  s = config.getData(Config::DrawMode);
//  action = new QAction(tr("Toggle Draw Mode"), icon, tr("Toggle Dra&w Mode"), CTRL+Key_W, this, 0, true);
  action = new QAction(icon, tr("Toggle Dra&w Mode"), 0, this, "actionDraw");
  action->setStatusTip(tr("Toggle drawing mode."));
  action->setToggleAction(TRUE);
  action->setOn(s.toInt());
  connect(action, SIGNAL(toggled(bool)), this, SIGNAL(signalDraw(bool)));
  actions.replace(DrawMode, action);
  
  icon = help;
//  action = new QAction(tr("Help"), icon, tr("&Help..."), CTRL+Key_H, this);
  action = new QAction(icon, tr("&Help..."), 0, this, "actionHelp");
  action->setStatusTip(tr("Display Help Dialog."));
  connect(action, SIGNAL(activated()), mw, SLOT(slotHelp()));
  actions.replace(Help, action);
  
  
  
  // sets a key accel for setting main plot focus
  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_C);
  connect(action, SIGNAL(activated()), this, SLOT(slotChartPanelFocus()));
  actions.replace(ChartPanelFocus, action);

  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_G);
  connect(action, SIGNAL(activated()), this, SLOT(slotGroupPanelFocus()));
  actions.replace(GroupPanelFocus, action);
  
  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_I);
  connect(action, SIGNAL(activated()), this, SLOT(slotIndicatorPanelFocus()));
  actions.replace(IndicatorPanelFocus, action);

  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_P);
  connect(action, SIGNAL(activated()), this, SLOT(slotPortfolioPanelFocus()));
  actions.replace(PortfolioPanelFocus, action);

  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_T);
  connect(action, SIGNAL(activated()), this, SLOT(slotTestPanelFocus()));
  actions.replace(TestPanelFocus, action);

  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_S);
  connect(action, SIGNAL(activated()), this, SLOT(slotScannerPanelFocus()));
  actions.replace(ScannerPanelFocus, action);

  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_M);
  connect(action, SIGNAL(activated()), this, SLOT(slotMacroPanelFocus()));
  actions.replace(MacroPanelFocus, action);
          
  action = new QAction(mw, 0, FALSE);
  action->setAccel(CTRL+Key_B);
  connect(action, SIGNAL(activated()), mw, SLOT(slotToolbarFocus()));
  actions.replace(ToolbarFocus, action);
  
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
  Config config;
  
  config.setData(Config::DrawMode, QString::number(getStatus(DrawMode)));
}

void MainMenubar::slotChartPanelFocus ()
{
  emit signalFocusEvent(0);
}

void MainMenubar::slotGroupPanelFocus()
{
  emit signalFocusEvent(1);
}

void MainMenubar::slotIndicatorPanelFocus()
{
  emit signalFocusEvent(2);
}

void MainMenubar::slotPortfolioPanelFocus()
{
  emit signalFocusEvent(3);
}

void MainMenubar::slotTestPanelFocus()
{
  emit signalFocusEvent(4);
}

void MainMenubar::slotScannerPanelFocus()
{
  emit signalFocusEvent(5);
}

void MainMenubar::slotMacroPanelFocus()
{
  emit signalFocusEvent(6);
}

