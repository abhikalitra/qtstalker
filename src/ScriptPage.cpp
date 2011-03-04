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
#include "ScriptDataBase.h"
#include "Globals.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/new.xpm"
#include "../pics/script.xpm"
#include "../pics/configure.xpm"
#include "../pics/cancel.xpm"

#include <QCursor>
#include <QToolTip>
#include <QPixmap>
#include <QVBoxLayout>
#include <QIcon>
#include <QDebug>
#include <QLabel>
#include <QListWidgetItem>
#include <QToolButton>
#include <QSettings>
#include <QInputDialog>
#include <QFileDialog>

ScriptPage::ScriptPage (QWidget *p) : QWidget (p)
{
  createActions();

  createGUI();

  connect(&_timer, SIGNAL(timeout()), this, SLOT(scriptTimer()));
  _timer.start(60000);
  
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
      connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScript(QString)));
      tb->addWidget(b);
    }
  }
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(new_xpm), tr("&New Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
  action->setToolTip(tr("New Script") + "...");
  action->setStatusTip(tr("New Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  _actions.insert(_NewScript, action);

  action = new QAction(QIcon(edit_xpm), tr("&Edit Script") + "...", this);
  action->setToolTip(tr("Edit Script") + "...");
  action->setStatusTip(tr("Edit Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  _actions.insert(_EditScript, action);

  action = new QAction(QIcon(delete_xpm), tr("&Delete Script") + "...", this);
  action->setToolTip(tr("Delete Script") + "...");
  action->setStatusTip(tr("Delete Script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  _actions.insert(_DeleteScript, action);

  action = new QAction(QIcon(cancel_xpm), tr("&Cancel Script") + "...", this);
  action->setToolTip(tr("Cancel script") + "...");
  action->setStatusTip(tr("Cancel script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(cancelScript()));
  _actions.insert(_CancelScript, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
  action->setToolTip(tr("Run script") + "...");
  action->setStatusTip(tr("Run script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(runScript()));
  _actions.insert(_RunScript, action);

  action = new QAction(QIcon(script_xpm), tr("Run E&xternal Script") + "...", this);
  action->setShortcut(QKeySequence(Qt::ALT + Qt::Key_R));
  action->setToolTip(tr("Run external script") + "...");
  action->setStatusTip(tr("Run external script") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(fileSelect()));
  _actions.insert(_RunExternalScript, action);

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
  _queMenu->addAction(_actions.value(_RunExternalScript));
  _queMenu->addAction(_actions.value(_CancelScript));
  _queMenu->addSeparator();
  _queMenu->addAction(_actions.value(_NewScript));
  _queMenu->addAction(_actions.value(_EditScript));
  _queMenu->addAction(_actions.value(_DeleteScript));
  _queMenu->addSeparator();
  _queMenu->addAction(_actions.value(_LaunchButtonRows));
  _queMenu->addAction(_actions.value(_LaunchButtonCols));
}

void ScriptPage::newScript ()
{
  QSettings settings(g_globalSettings);
  
  Script *script = new Script(this);
  script->setName("ScriptPanelNewScript");
  script->setFile(settings.value("script_panel_new_script_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ScriptPage::editScript ()
{
  QSettings settings(g_globalSettings);
  
  Script *script = new Script(this);
  script->setName("ScriptPanelEditScript");
  script->setFile(settings.value("script_panel_edit_script_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ScriptPage::deleteScript ()
{
  QSettings settings(g_globalSettings);
  
  Script *script = new Script(this);
  script->setName("ScriptPanelDeleteScript");
  script->setFile(settings.value("script_panel_delete_script_script").toString());
  script->setCommand("perl");
  script->startScript();
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
  
  _actions.value(_CancelScript)->setEnabled(status); 
}

void ScriptPage::runScript ()
{
  QSettings settings(g_globalSettings);
  
  Script *script = new Script(this);
  script->setName("ScriptPanelRunScript");
  script->setFile(settings.value("script_panel_run_script_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ScriptPage::runScript (QString d)
{
  Script *script = new Script(this);
  setupScript(script);
  script->setName(d);

  ScriptDataBase db;
  if (db.load(script))
  {
    qDebug() << "ScriptPage::runScript: ScriptDataBase error" << d;
    delete script;
    return;
  }
  
  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(d);
  _itemList.insert(d, item);

  QStringList l;
  l << tr("Script") << d << tr("started");
  g_middleMan->statusMessage(l.join(" "));
  
  script->startScript();
}

void ScriptPage::runExternalScript (QString file)
{
  QSettings settings(g_localSettings);
  settings.setValue("script_panel_last_external_script", file);
  settings.sync();

  QFileInfo fi(file);
  
  Script *script = new Script(this);
  setupScript(script);
  script->setName(fi.baseName());
  script->setFile(file);
  script->setCommand("perl");

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(fi.baseName());
  _itemList.insert(fi.baseName(), item);
  
  QStringList l;
  l << tr("Script") << script->name() << tr("started");
  g_middleMan->statusMessage(l.join(" "));

  script->startScript();
}

void ScriptPage::cancelScript ()
{
  QList<QListWidgetItem *> sl = _queList->selectedItems();
  if (! sl.count())
    return;

  emit signalCancelScript(sl.at(0)->text());
}

void ScriptPage::cancelScript (QString d)
{
  emit signalCancelScript(d);
}

void ScriptPage::done (QString name)
{
  QListWidgetItem *item = _itemList.value(name);
  if (! item)
    return;

  _itemList.remove(name);
  delete item;

  QStringList l;
  l << tr("Script") << name << tr("ended");
  g_middleMan->statusMessage(l.join(" "));

  ScriptDataBase db;
  Script script(this);
  script.setName(name);
  if (db.load(&script))
  {
    // if an external script has run, we are ok to exit here
//    qDebug() << "ScriptPage::done: script load error" << name;
    return;
  }

  script.setLastRun(QDateTime::currentDateTime().toString(Qt::ISODate));
  
  if (db.save(&script))
    qDebug() << "ScriptPage::done: script save error" << name;
}

void ScriptPage::launchButtonRows ()
{
  QSettings settings(g_localSettings);
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_rows", 2).toInt());
  dialog->setLabelText(tr("Rows"));
  dialog->setWindowTitle(tr("Configure Launch Button Rows"));
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

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_cols", 4).toInt());
  dialog->setLabelText(tr("Columns"));
  dialog->setWindowTitle(tr("Configure Launch Button Columns"));
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(launchButtonCols2(int)));
  dialog->show();
}

void ScriptPage::launchButtonCols2 (int d)
{
  QSettings settings(g_localSettings);
  settings.setValue("script_launch_button_cols", d);
  settings.sync();
}

void ScriptPage::scriptTimer ()
{
  ScriptDataBase db;
  QStringList l;
  db.timerScripts(l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    Script script(this);
    script.setName(l.at(loop));
    db.load(&script);

    QDateTime dt = QDateTime::currentDateTime();
    QDateTime dt2 = QDateTime::fromString(script.lastRun(), Qt::ISODate);
    int secs = dt2.secsTo(dt);
    if (secs / 60 < script.minutes())
      continue;

    runScript(l.at(loop));    
  }

  queStatus();
}

void ScriptPage::fileSelect ()
{
  QSettings settings(g_localSettings);
  
  QFileDialog *dialog = new QFileDialog;
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("Select External Script"));
  dialog->setDirectory(settings.value("script_panel_last_external_script").toString());
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(runExternalScript(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void ScriptPage::setupScript (Script *script)
{
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  connect(script, SIGNAL(signalStopped(QString)), this, SLOT(done(QString)));
  connect(this, SIGNAL(signalCancelScript(QString)), script, SLOT(stopScript(QString)));
}

void ScriptPage::shutDown ()
{
  emit signalCancelScript(QString());
}

QListWidget * ScriptPage::list ()
{
  return _queList;
}
