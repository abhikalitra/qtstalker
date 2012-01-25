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
#include "GlobalSymbol.h"
#include "GlobalParent.h"
#include "DialogNew.h"
#include "DialogSelect.h"
#include "ScriptTimerDialog.h"
#include "ScriptRunDialog.h"
#include "ThreadMessage.h"
#include "Global.h"
#include "EAVDataBase.h"
#include "EAVSearch.h"
#include "KeyScriptDataBase.h"
#include "ScriptTimerAdd.h"
#include "ScriptTimerModified.h"
#include "ScriptTimerRemove.h"

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

ScriptPage::ScriptPage (QWidget *p) : QWidget (p)
{
  createActions();

  createGUI();

  runStartupScripts();

  setupScriptTimers();

  queStatus();
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
      connect(b, SIGNAL(signalButtonClicked(QString, QString)), this, SLOT(runScript(QString, QString)));
      tb->addWidget(b);
    }
  }
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(new_xpm), tr("New Script Timer") + "...", this);
  action->setToolTip(tr("New Script Timer") + "...");
  action->setStatusTip(tr("New Script Timer") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newScriptTimer()));
  _actions.insert(_NewScriptTimer, action);

  action = new QAction(QIcon(edit_xpm), tr("Edit Script Timer") + "...", this);
  action->setToolTip(tr("Edit Script Timer") + "...");
  action->setStatusTip(tr("Edit Script Timer") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(editScriptTimer()));
  _actions.insert(_EditScriptTimer, action);

  action = new QAction(QIcon(delete_xpm), tr("Delete Script Timer") + "...", this);
  action->setToolTip(tr("Delete Script Timer") + "...");
  action->setStatusTip(tr("Delete Script Timer") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteScriptTimer()));
  _actions.insert(_DeleteScriptTimer, action);

  action = new QAction(QIcon(cancel_xpm), tr("&Cancel") + "...", this);
  action->setToolTip(tr("Cancel") + "...");
  action->setStatusTip(tr("Cancel") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(cancel()));
  _actions.insert(_Cancel, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  action->setToolTip(tr("Run Script") + "...");
  action->setStatusTip(tr("Run Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(runScript()));
  _actions.insert(_RunScript, action);

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
  _queMenu->addAction(_actions.value(_NewScriptTimer));
  _queMenu->addAction(_actions.value(_EditScriptTimer));
  _queMenu->addAction(_actions.value(_DeleteScriptTimer));
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

void ScriptPage::runScript ()
{
  QSettings settings(g_localSettings);
  QStringList l;
  l << settings.value("script_panel_last_external_script").toString();

  ScriptRunDialog *dialog = new ScriptRunDialog(this,
                                                settings.value("script_panel_last_external_script").toString(),
                                                QString("perl"));
  connect(dialog, SIGNAL(signalDone(QString, QString)), this, SLOT(runScript(QString, QString)));
  dialog->show();
}

void ScriptPage::runScript (QString command, QString file)
{
  QFileInfo fi(file);

  Script *script = new Script(this);
  script->setName(fi.baseName());
  script->setFile(file);
  script->setCommand(command);
  script->setSymbol(g_currentSymbol);
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  connect(script, SIGNAL(signalMessage(QString)), this, SLOT(scriptThreadMessage(QString)));
  _scripts.insert(script->id(), script);

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(fi.fileName());
  _itemList.insert(script->id(), item);

//  QStringList l;
//  l << tr("Script") << file << tr("started");
//  g_parent->statusBar()->showMessage(l.join(" "));

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

  done(sl.at(0)->text());
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

void ScriptPage::setupScriptTimers ()
{
  KeyScriptDataBase skeys;
  EAVSearch search;
  search.append(skeys.indexToString(KeyScriptDataBase::_RUN_INTERVAL), ">", "0");

  EAVDataBase db("scripts");
  QStringList l;
  if (db.search(search, l))
  {
    qDebug() << "ScriptPage::setupScriptTimers: db error";
    return;
  }

  ScriptTimerAdd sta;
  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Entity st;
    st.setName(l.at(loop));

    if (db.get(st))
      continue;

    sta.add(st);
  }
}

QListWidget * ScriptPage::list ()
{
  return _queList;
}

void ScriptPage::newScriptTimer ()
{
  EAVDataBase db("scripts");
  QStringList sl;
  if (db.names(sl))
  {
    qDebug() << "ScriptPage::newScriptTimer: db error";
    return;
  }

  WindowTitle wt;
  DialogNew *dialog = new DialogNew(this, QString(), Entity());
  dialog->setWindowTitle(wt.title(tr("New Script Timer"), QString()));
  dialog->setTitle(tr("Enter script timer name"));
  dialog->setItems(sl);
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(editScriptTimer(QString)));
  dialog->show();
}

void ScriptPage::editScriptTimer ()
{
  WindowTitle wt;
  DialogSelect *dialog = new DialogSelect(this, QString(), Entity());
  dialog->setWindowTitle(wt.title(tr("Select script timer to edit"), QString()));

  QStringList l;
  EAVDataBase db("scripts");
  if (db.names(l))
  {
    qDebug() << "ScriptPage::editScriptTimer: db error";
    return;
  }

  dialog->setItems(l);
  dialog->setTitle(tr("Scripts"));
  dialog->setMode(1);
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(editScriptTimer2(QStringList)));
  dialog->show();
}

void ScriptPage::editScriptTimer (QString d)
{
  ScriptTimerDialog *dialog = new ScriptTimerDialog(this, d);
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(editScriptTimer3(QString)));
  dialog->show();
}

void ScriptPage::editScriptTimer2 (QStringList l)
{
  if (! l.count())
    return;

  editScriptTimer(l.at(0));
}

void ScriptPage::editScriptTimer3 (QString d)
{
  ScriptTimerModified stm;
  stm.modified(d);
}

void ScriptPage::deleteScriptTimer ()
{
  WindowTitle wt;
  DialogSelect *dialog = new DialogSelect(this, QString(), Entity());
  dialog->setWindowTitle(wt.title(tr("Select script timer to delete"), QString()));

  QStringList l;
  EAVDataBase db("scripts");
  db.names(l);
  dialog->setItems(l);

  dialog->setTitle(tr("Scripts"));
  dialog->setMode(1);
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(deleteScriptTimer2(QStringList)));
  dialog->show();
}

void ScriptPage::deleteScriptTimer2 (QStringList l)
{
  ScriptTimerRemove str;
  str.remove(l);
}

void ScriptPage::runStartupScripts ()
{
  KeyScriptDataBase skeys;
  EAVSearch search;
  search.append(skeys.indexToString(KeyScriptDataBase::_STARTUP), "=", "true");

  EAVDataBase db("scripts");
  QStringList l;
  if (db.search(search, l))
  {
    qDebug() << "ScriptPage::runStartupScripts: db error";
    return;
  }

  int loop = 0;
  for (; loop < l.size(); loop++)
  {
    Entity data;
    data.setName(l.at(loop));

    if (db.get(data))
      continue;

    Data td;
    data.toData(skeys.indexToString(KeyScriptDataBase::_COMMAND), td);
    runScript(td.toString(), l.at(loop));
  }
}

void ScriptPage::scriptThreadMessage (QString id)
{
  ThreadMessage tm;
  tm.getMessage(id);
}
