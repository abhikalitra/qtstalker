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

#include "PlotMenu.h"
#include "Globals.h"
#include "Script.h"
#include "ChartObjectDataBase.h"
#include "ScriptPluginFactory.h"

#include "../pics/loggrid.xpm"
#include "../pics/date.xpm"
#include "../pics/delete.xpm"
#include "../pics/buyarrow.xpm"
#include "../pics/sellarrow.xpm"
#include "../pics/fib.xpm"
#include "../pics/horizontal.xpm"
#include "../pics/text.xpm"
#include "../pics/trend.xpm"
#include "../pics/vertical.xpm"
#include "../pics/edit.xpm"

#include <QtDebug>
#include <QSettings>

PlotMenu::PlotMenu (QWidget *p) : QMenu (p)
{
  init ();
}

PlotMenu::~PlotMenu ()
{
  delete _command;
}

void PlotMenu::init ()
{
  _coListMenu = new QMenu(this);
  _coListMenu->setTitle(tr("New Chart Object..."));
  
  _coListMenu->addAction(QPixmap(buyarrow_xpm), tr("Buy"));
  _coListMenu->addAction(QPixmap(horizontal_xpm), tr("HLine"));
  _coListMenu->addAction(QPixmap(fib_xpm), tr("Retracement"));
  _coListMenu->addAction(QPixmap(sellarrow_xpm), tr("Sell"));
  _coListMenu->addAction(QPixmap(text_xpm), tr("Text"));
  _coListMenu->addAction(QPixmap(trend_xpm), tr("TLine"));
  _coListMenu->addAction(QPixmap(vertical_xpm), tr("VLine"));
  
  connect(_coListMenu, SIGNAL(triggered(QAction *)), this, SLOT(chartObjectMenuSelected(QAction *)));
  
//  addAction(QPixmap(indicator_xpm), tr("&New Indicator..."), this, SIGNAL(signalNewIndicator()), Qt::ALT+Qt::Key_N);
  
  addAction(QPixmap(edit_xpm), tr("Edit &Indicator..."), this, SLOT(editIndicator()), Qt::ALT+Qt::Key_I);
  
  addAction(QPixmap(delete_xpm), tr("De&lete Indicator..."), this, SLOT(deleteIndicator()), Qt::ALT+Qt::Key_L);
  
  addSeparator ();

  // chart object section
  addMenu(_coListMenu);
  
  addAction(QPixmap(edit_xpm), tr("Edit &Chart Object"), this, SLOT(editChartObject()), Qt::ALT+Qt::Key_C);

  addAction(QPixmap(delete_xpm), tr("&Delete Chart Objects"), this, SLOT(deleteChartObject()), Qt::ALT+Qt::Key_D);
  
  addSeparator ();
  
  _dateAction = addAction(QPixmap(date_xpm), tr("&Date"));
  _dateAction->setCheckable(TRUE);
  connect(_dateAction, SIGNAL(toggled(bool)), this, SIGNAL(signalDateStatus(bool)));
  
  _logAction = addAction(QPixmap(loggrid_xpm), tr("Log &Scaling"));
  _logAction->setCheckable(TRUE);
  connect(_logAction, SIGNAL(toggled(bool)), this, SIGNAL(signalLogStatus(bool)));

  addSeparator ();

  _lockAction = addAction(QPixmap(), tr("&Locked"));
  _lockAction->setCheckable(TRUE);
  _lockAction->setChecked(FALSE);
  connect(_lockAction, SIGNAL(toggled(bool)), this, SIGNAL(signalLockStatus(bool)));

  _command = new Command;
}

void PlotMenu::editIndicator ()
{
  QSettings settings(g_settingsFile);
  
  Script *script = new Script;
  script->setName("IndicatorEdit");
  script->setFile(settings.value("indicator_edit_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::deleteIndicator ()
{
  QSettings settings(g_settingsFile);
  
  Script *script = new Script;
  script->setName("IndicatorDelete");
  script->setFile(settings.value("indicator_delete_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::deleteAllChartObjects ()
{
  QSettings settings(g_settingsFile);

  Script *script = new Script;
  script->setName("ChartObjectDeleteAll");
  script->setFile(settings.value("chart_object_delete_all_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::chartObjectMenuSelected (QAction *a)
{
  _currentAction = a;

  ChartObjectDataBase db;
  QStringList l2;
  db.ids(g_barData, _indicator, l2);

  QStringList cl;
  cl << "PLUGIN=NEW_DIALOG" << "TITLE=" + tr("chart object") << "ITEMS=" + l2.join(";");
  _command->parse(cl.join(","));

  ScriptPluginFactory fac;
  ScriptPlugin *plug = fac.plugin(_command->plugin());
  if (! plug)
  {
    qDebug() << "PlotMenu::chartObjectMenuSelected: no plugin" << _command->plugin();
    return;
  }

  connect(plug, SIGNAL(signalResume()), this, SLOT(chartObjectMenuSelected2()));

  plug->command(_command);
}

void PlotMenu::chartObjectMenuSelected2 ()
{
  QString s = _command->returnData("NEW_DIALOG_NAME");
  if (s.isEmpty())
    return;
  
  emit signalNewChartObject(_currentAction->text(), s);
}

void PlotMenu::setCOMenuStatus (bool status)
{
  _coListMenu->setEnabled(status);
}

void PlotMenu::setLog (bool status)
{
  _logAction->setChecked(status);
}

bool PlotMenu::log ()
{
  return _logAction->isChecked();
}

void PlotMenu::setDate (bool status)
{
  _dateAction->setChecked(status);
}

bool PlotMenu::date ()
{
  return _dateAction->isChecked();
}

void PlotMenu::setLock (bool status)
{
  _lockAction->setChecked(status);
}

bool PlotMenu::lock ()
{
  return _lockAction->isChecked();
}

void PlotMenu::setIndicator (QString d)
{
  _indicator = d;
}

void PlotMenu::editChartObject ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("chart_object_edit_data", _indicator);
  settings.sync();
  
  Script *script = new Script;
  script->setName("ChartObjectEditSelect");
  script->setFile(settings.value("chart_object_edit_select_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void PlotMenu::deleteChartObject ()
{
  QSettings settings(g_settingsFile);
  settings.setValue("chart_object_delete_data", _indicator);
  settings.sync();

  Script *script = new Script;
  script->setName("DeleteObjectDeleteSelect");
  script->setFile(settings.value("chart_object_delete_select_script").toString());
  script->setCommand("perl");
  script->startScript();
}
