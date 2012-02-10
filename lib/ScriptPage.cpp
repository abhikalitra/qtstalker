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
#include "WindowTitle.h"
#include "GlobalParent.h"
#include "ScriptRunDialog.h"
#include "ThreadMessage.h"
#include "Global.h"
#include "EAVDataBase.h"
#include "EAVSearch.h"
#include "KeyScriptDataBase.h"
#include "ScriptFunctions.h"
#include "DialogEAVDataBaseSearch.h"
#include "KeyScriptLaunchButton.h"
#include "ScriptLaunchButtonDialog.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/new.xpm"
#include "../pics/script.xpm"
#include "../pics/cancel.xpm"

#include <QDebug>
#include <QSettings>
#include <QStatusBar>
#include <QDir>
#include <QTabWidget>
#include <QUuid>

ScriptPage::ScriptPage (QWidget *p) : QWidget (p)
{
  createActions();

  createGUI();
  
  queStatus();
  
  updateSearchList();
  
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

  // create tabs
  QTabWidget *tabs = new QTabWidget;
  vbox->addWidget(tabs);
  
  // create script search tab
  QWidget *w = new QWidget;
  
  QVBoxLayout *tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  w->setLayout(tvbox);
  
  _searchList = new QListWidget;
  _searchList->setContextMenuPolicy(Qt::CustomContextMenu);
  _searchList->setSortingEnabled(TRUE);
  connect(_searchList, SIGNAL(itemSelectionChanged()), this, SLOT(searchStatus()));
  connect(_searchList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(searchRightClick(const QPoint &)));
  connect(_searchList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(searchDoubleClicked(QListWidgetItem *)));
  tvbox->addWidget(_searchList);
  tabs->addTab(w, tr("Scripts"));
  
  // create running script tab
  w = new QWidget;
  
  tvbox = new QVBoxLayout;
  tvbox->setMargin(0);
  tvbox->setSpacing(0);
  w->setLayout(tvbox);

  _queList = new QListWidget;
  _queList->setContextMenuPolicy(Qt::CustomContextMenu);
  _queList->setSortingEnabled(TRUE);
  connect(_queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(_queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  tvbox->addWidget(_queList);
  tabs->addTab(w, tr("Active"));

  // create launch buttons
  _buttonGrid = new QGridLayout;
  _buttonGrid->setMargin(0);
  _buttonGrid->setSpacing(2);
  vbox->addLayout(_buttonGrid);
  
  EAVDataBase db("scriptLaunchButtons");
  QStringList tl;
  db.names(tl);
  
  KeyScriptLaunchButton keys;
  int loop = 0;
  for (; loop < tl.size(); loop++)
  {
    Entity settings;
    settings.setName(tl.at(loop));
    if (db.get(settings))
      continue;
    
    ScriptLaunchButton *b = new ScriptLaunchButton(tl.at(loop));
    connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScriptInternal(QString)));
    connect(b, SIGNAL(signalButtonRemove(QString)), this, SLOT(deleteLaunchButton(QString)));
    _launchButtons.insert(tl.at(loop), b);

    _buttonGrid->addWidget(b, b->row(), b->col());
  }

  // left justify buttons
  _buttonGrid->setColumnStretch(_buttonGrid->columnCount(), 1);
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(cancel_xpm), tr("&Cancel") + "...", this);
  action->setToolTip(tr("Cancel") + "...");
  action->setStatusTip(tr("Cancel") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(cancel()));
  _actions.insert(_SCRIPT_CANCEL, action);

  action = new QAction(QIcon(script_xpm), tr("&Search") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  action->setToolTip(tr("Search") + "...");
  action->setStatusTip(tr("Search") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(searchDialog()));
  _actions.insert(_SCRIPT_SEARCH, action);

  action = new QAction(QIcon(script_xpm), tr("Show &All Scripts"), this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
  action->setToolTip(tr("Show All Scripts"));
  action->setStatusTip(tr("Show All Scripts"));
  connect(action, SIGNAL(activated()), this, SLOT(showAll()));
  _actions.insert(_SCRIPT_SHOW_ALL, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  action->setToolTip(tr("Run Script") + "...");
  action->setStatusTip(tr("Run Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(runScriptDialog()));
  _actions.insert(_SCRIPT_RUN, action);

  action = new QAction(QIcon(new_xpm), tr("&New Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
  action->setToolTip(tr("New Script") + "...");
  action->setStatusTip(tr("New Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  _actions.insert(_SCRIPT_NEW, action);

  action = new QAction(QIcon(edit_xpm), tr("&Edit Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
  action->setToolTip(tr("Edit Script") + "...");
  action->setStatusTip(tr("Edit Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  _actions.insert(_SCRIPT_EDIT, action);

  action = new QAction(QIcon(delete_xpm), tr("&Delete Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));
  action->setToolTip(tr("Delete Script") + "...");
  action->setStatusTip(tr("Delete Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  _actions.insert(_SCRIPT_DELETE, action);

  action = new QAction(QIcon(new_xpm), tr("New Launch Button") + "...", this);
  action->setToolTip(tr("New Launch Button") + "...");
  action->setStatusTip(tr("New Launch Button") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newLaunchButton()));
  _actions.insert(_LAUNCH_BUTTON_NEW, action);
}

void ScriptPage::createButtonMenu ()
{
  _queMenu = new QMenu(this);
  _queMenu->addAction(_actions.value(_SCRIPT_CANCEL));
  
  _searchMenu = new QMenu(this);
  _searchMenu->addAction(_actions.value(_SCRIPT_RUN));
  _searchMenu->addSeparator();
  _searchMenu->addAction(_actions.value(_SCRIPT_SHOW_ALL));
  _searchMenu->addAction(_actions.value(_SCRIPT_SEARCH));
  _searchMenu->addSeparator();
  _searchMenu->addAction(_actions.value(_SCRIPT_NEW));
  _searchMenu->addAction(_actions.value(_SCRIPT_EDIT));
  _searchMenu->addAction(_actions.value(_SCRIPT_DELETE));
  _searchMenu->addSeparator();
  _searchMenu->addAction(_actions.value(_LAUNCH_BUTTON_NEW));
}

void ScriptPage::queRightClick (const QPoint &)
{
  _queMenu->exec(QCursor::pos());
}

void ScriptPage::searchRightClick (const QPoint &)
{
  _searchMenu->exec(QCursor::pos());
}

void ScriptPage::queStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _queList->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(_SCRIPT_CANCEL)->setEnabled(status);
}

void ScriptPage::searchStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _searchList->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(_SCRIPT_RUN)->setEnabled(status);
  _actions.value(_SCRIPT_EDIT)->setEnabled(status);
  _actions.value(_SCRIPT_DELETE)->setEnabled(status);
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
  QList<QListWidgetItem *> sel = _searchList->selectedItems();
  if (! sel.count())
    return;

  runScriptInternal(sel.at(0)->text());  
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

  QHashIterator<QString, Script *> it(_scripts);
  while (it.hasNext())
  {
    it.next();
    Script *script = it.value();
    if (script->name() == sl.at(0)->text())
    {
      script->stopScript();
      break;
    }
  }
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

void ScriptPage::updateSearchList ()
{
  showAll();
}

void ScriptPage::searchDialog ()
{
  DialogEAVDataBaseSearch *dialog = new DialogEAVDataBaseSearch(this);
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(setSearchList(QStringList)));
  dialog->show();
}

void ScriptPage::showAll ()
{
  ScriptFunctions sf;
  QStringList l;
  sf.names(l);
  
  _searchList->clear();
  _searchList->addItems(l);
  
  searchStatus();
}

void ScriptPage::setSearchList (QStringList l)
{
  _searchList->clear();
  _searchList->addItems(l);
  searchStatus();
}

void ScriptPage::newLaunchButton ()
{
  ScriptLaunchButtonDialog *dialog = new ScriptLaunchButtonDialog(this, QString(), QString(), FALSE, 0, 0, QString());
  connect(dialog, SIGNAL(signalDone(QString, QString, bool, int, int, QString)),
	  this, SLOT(newLaunchButton2(QString, QString, bool, int, int, QString)));
  dialog->show();
}

void ScriptPage::newLaunchButton2 (QString script, QString icon, bool flag, int row, int col, QString text)
{
  ScriptLaunchButton *b = new ScriptLaunchButton(QString());
  connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScriptInternal(QString)));
  connect(b, SIGNAL(signalButtonRemove(QString)), this, SLOT(deleteLaunchButton(QString)));
  b->configure2(script, icon, flag, row, col, text);
  _launchButtons.insert(b->name(), b);
  _buttonGrid->addWidget(b, b->row(), b->col());
  
  // left justify buttons
  _buttonGrid->setColumnStretch(_buttonGrid->columnCount(), 1);
}

void ScriptPage::deleteLaunchButton (QString d)
{
  QStringList tl;
  tl << d;
  
  EAVDataBase db("scriptLaunchButtons");
  if (db.remove(tl))
    qDebug() << "ScriptPage::deleteLaunchButton: db error";
  
  ScriptLaunchButton *b = _launchButtons.value(d);
  if (! b)
  {
    qDebug() << "ScriptPage::deleteLaunchButton: button not found";
    return;
  }
  
  delete b;
  _launchButtons.remove(d);
}

void ScriptPage::searchDoubleClicked (QListWidgetItem *d)
{
  if (! d)
    return;
  
  runScriptInternal(d->text());  
}

void ScriptPage::selected (QStringList &l)
{
  l.clear();

  QList<QListWidgetItem *> sel = _searchList->selectedItems();
  if (! sel.count())
    return;

  int loop = 0;
  for (; loop < sel.count(); loop++)
    l << sel.at(loop)->text();
}
