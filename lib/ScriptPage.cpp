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

#include "ScriptPage.h"
#include "ScriptLaunchButton.h"
#include "WindowTitle.h"
#include "GlobalParent.h"
#include "ScriptRunDialog.h"
#include "ThreadMessage.h"
#include "Global.h"
#include "EAVDataBase.h"
#include "EAVSearch.h"
#include "KeyScriptDataBase.h"
#include "ScriptFunctions.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/new.xpm"
#include "../pics/script.xpm"
#include "../pics/configure.xpm"
#include "../pics/cancel.xpm"

#include <QDebug>
#include <QSettings>
#include <QInputDialog>
#include <QStatusBar>
#include <QDir>

ScriptPage::ScriptPage (QWidget *p) : QWidget (p)
{
  createActions();

  createGUI();
  
  queStatus();
  
  _timer.setInterval(60000);
  connect(&_timer, SIGNAL(timeout()), this, SLOT(runIntervalScripts()));
  _timer.start();
}

void ScriptPage::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  createButtonMenu();

  _queList = new QListWidget;
  _queList->setContextMenuPolicy(Qt::CustomContextMenu);
  _queList->setSortingEnabled(TRUE);
  connect(_queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(_queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(_queList);

  // create launch buttons
  QSettings settings(g_localSettings);
  int rows = settings.value("script_launch_button_rows", 2).toInt();
  int cols = settings.value("script_launch_button_cols", 5).toInt();

  int loop = 0;
  int pos = 1;
  for (; loop < rows; loop++)
  {
    QToolBar *tb = new QToolBar;
    vbox->addWidget(tb);
    tb->setIconSize(QSize(16, 16));

    int loop2 = 0;
    for (; loop2 < cols; loop2++, pos++)
    {
      ScriptLaunchButton *b = new ScriptLaunchButton(pos);
      connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScriptInternal(QString)));
      tb->addWidget(b);
    }
  }
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(cancel_xpm), tr("&Cancel") + "...", this);
  action->setToolTip(tr("Cancel") + "...");
  action->setStatusTip(tr("Cancel") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(cancel()));
  _actions.insert(_Cancel, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  action->setToolTip(tr("Run Script") + "...");
  action->setStatusTip(tr("Run Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(runScriptDialog()));
  _actions.insert(_RunScript, action);

  action = new QAction(QIcon(new_xpm), tr("&New Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  action->setToolTip(tr("New Script") + "...");
  action->setStatusTip(tr("New Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  _actions.insert(_ScriptNew, action);

  action = new QAction(QIcon(edit_xpm), tr("&Edit Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  action->setToolTip(tr("Edit Script") + "...");
  action->setStatusTip(tr("Edit Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  _actions.insert(_ScriptEdit, action);

  action = new QAction(QIcon(delete_xpm), tr("&Delete Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
  action->setToolTip(tr("Delete Script") + "...");
  action->setStatusTip(tr("Delete Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  _actions.insert(_ScriptDelete, action);

  action = new QAction(QIcon(configure_xpm), tr("Set Launch Button Rows") + "...", this);
  action->setToolTip(tr("Set Launch Button Rows") + "...");
  action->setStatusTip(tr("Set Launch Button Rows") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(launchButtonRows()));
  _actions.insert(_LaunchButtonRows, action);

  action = new QAction(QIcon(configure_xpm), tr("Set Launch Button Columns") + "...", this);
  action->setToolTip(tr("Set Launch Button Columns") + "...");
  action->setStatusTip(tr("Set Launch Button Columns") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(launchButtonCols()));
  _actions.insert(_LaunchButtonCols, action);
}

void ScriptPage::createButtonMenu ()
{
  _queMenu = new QMenu(this);
  _queMenu->addAction(_actions.value(_RunScript));
  _queMenu->addAction(_actions.value(_Cancel));
  _queMenu->addSeparator();
  _queMenu->addAction(_actions.value(_ScriptNew));
  _queMenu->addAction(_actions.value(_ScriptEdit));
  _queMenu->addAction(_actions.value(_ScriptDelete));
  _queMenu->addSeparator();
  _queMenu->addAction(_actions.value(_LaunchButtonRows));
  _queMenu->addAction(_actions.value(_LaunchButtonCols));
}

void ScriptPage::queRightClick (const QPoint &)
{
  _queMenu->exec(QCursor::pos());
}

void ScriptPage::queStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _queList->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(_Cancel)->setEnabled(status);
}

void ScriptPage::runScriptSystem (QString file)
{
  QDir dir;
  if (! dir.exists(file))
    return;
  
  Script *script = new Script(g_parent);
  connect(script, SIGNAL(signalMessage(QString)), this, SLOT(scriptThreadMessage(QString)));
  script->setName(file);
  script->setFile(file);
  script->setCommand(QString("perl"));
  script->start();
}

void ScriptPage::runScriptDialog ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptRun.pl");
  runScriptSystem(file);
}

void ScriptPage::runScriptInternal (QString name)
{
  ScriptFunctions sf;
  Entity settings;
  settings.setName(name);
  if (sf.load(settings))
  {
    qDebug() << "ScriptPage::runScriptInternal: error loading script" << name;
    return;
  }

  KeyScriptDataBase keys;
  Data command, file;
  settings.toData(keys.indexToString(KeyScriptDataBase::_COMMAND), command);
  settings.toData(keys.indexToString(KeyScriptDataBase::_FILE), file);

  QDir dir;
  if (! dir.exists(file.toString()))
    return;
  
  QFileInfo fi(file.toString());
  
  Script *script = new Script(this);
  script->setName(name);
  script->setFile(file.toString());
  script->setCommand(command.toString());
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  connect(script, SIGNAL(signalMessage(QString)), this, SLOT(scriptThreadMessage(QString)));
  _scripts.insert(script->id(), script);

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(name);
  _itemList.insert(script->id(), item);

  QSettings qsettings(g_localSettings);
  qsettings.setValue("script_panel_last_internal_script", name);
  qsettings.sync();
  
  script->start();
}

void ScriptPage::runScriptExternal (QString command, QString file)
{
  QDir dir;
  if (! dir.exists(file))
    return;
  
  QFileInfo fi(file);

  Script *script = new Script(this);
  script->setName(fi.baseName());
  script->setFile(file);
  script->setCommand(command);
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  connect(script, SIGNAL(signalMessage(QString)), this, SLOT(scriptThreadMessage(QString)));
  _scripts.insert(script->id(), script);

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(fi.fileName());
  _itemList.insert(script->id(), item);

  QSettings settings(g_localSettings);
  settings.setValue("script_panel_last_external_script", file);
  settings.sync();
  
  script->start();
}

void ScriptPage::cancel ()
{
  QList<QListWidgetItem *> sl = _queList->selectedItems();
  if (! sl.count())
    return;

  Script *script = _scripts.value(sl.at(0)->text());
  if (! script)
    return;
  
  script->stopScript();
  
  done(sl.at(0)->text());
}

void ScriptPage::newScript ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptNew.pl");
  runScriptSystem(file);
}

void ScriptPage::editScript ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptEdit.pl");
  runScriptSystem(file);
}

void ScriptPage::deleteScript ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptDelete.pl");
  runScriptSystem(file);
}

void ScriptPage::done (QString id)
{
  QListWidgetItem *item = _itemList.value(id);
  if (! item)
    return;

  _itemList.remove(id);
  delete item;
  
  _scripts.remove(id);
}

void ScriptPage::launchButtonRows ()
{
  QSettings settings(g_localSettings);

  WindowTitle wt;
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_rows", 2).toInt());
  dialog->setLabelText(tr("Rows"));
  dialog->setWindowTitle(wt.title(tr("Set Launch Button Rows"), QString()));
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(launchButtonRows2(int)));
  dialog->show();
}

void ScriptPage::launchButtonRows2 (int d)
{
  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_rows", d);
  settings.sync();
}

void ScriptPage::launchButtonCols ()
{
  QSettings settings(g_localSettings);

  WindowTitle wt;
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_cols", 5).toInt());
  dialog->setLabelText(tr("Columns"));
  dialog->setWindowTitle(wt.title(tr("Set Launch Button Columns"), QString()));
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(launchButtonCols2(int)));
  dialog->show();
}

void ScriptPage::launchButtonCols2 (int d)
{
  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_cols", d);
  settings.sync();
}

QListWidget * ScriptPage::list ()
{
  return _queList;
}

void ScriptPage::runStartupScripts ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptStartupRun.pl");
  runScriptSystem(file);
}

void ScriptPage::scriptThreadMessage (QString id)
{
  ThreadMessage tm;
  tm.getMessage(id);
}

void ScriptPage::runIntervalScripts ()
{
  QSettings settings(g_globalSettings);
  QString file = settings.value("system_script_directory").toString();
  file.append("ScriptPanelScriptIntervalRun.pl");
  runScriptSystem(file);
}
