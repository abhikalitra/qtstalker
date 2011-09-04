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
#include "Globals.h"
#include "NewDialog.h"
#include "SelectDialog.h"
#include "ScriptDataBase.h"
#include "Script.h"
#include "ScriptTimerDialog.h"
#include "ScriptRunDialog.h"

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
#include <QStatusBar>
#include <QProcess>

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
      connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScript(QString)));
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

/*
  QFileDialog *dialog = new QFileDialog(this);
  dialog->setWindowTitle("QtStalker" + g_session + ": " + tr("Select Script"));
  dialog->setDirectory(settings.value("script_panel_last_external_script").toString());
  connect(dialog, SIGNAL(fileSelected(const QString &)), this, SLOT(runScript(QString)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
*/
}

void ScriptPage::runScript (QString file, QString command)
{
  QFileInfo fi(file);

  Script script(this);
  script.setName(fi.baseName());
  script.setFile(file);
  script.setCommand(command);
  script.setSession(g_session);

  if (script.run())
  {
    qDebug() << "ScriptPage::runScript: script error";
    return;
  }

  _pids.insert(script.name(), script.pid());

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(fi.baseName());
  _itemList.insert(fi.baseName(), item);

  QStringList l;
  l << tr("Script") << fi.baseName() << tr("started");
  g_parent->statusBar()->showMessage(l.join(" "));

  QSettings settings(g_localSettings);
  settings.setValue("script_panel_last_external_script", file);
  settings.sync();
}

void ScriptPage::cancel ()
{
  QList<QListWidgetItem *> sl = _queList->selectedItems();
  if (! sl.count())
    return;

  cancel(sl.at(0)->text());
}

void ScriptPage::cancel (QString d)
{
  if (! _pids.contains(d))
    return;

  QStringList args;
  args << QString::number(_pids.value(d));

  bool ok = QProcess::startDetached("kill", args);
  if (! ok)
  {
    qDebug() << "ScriptPage::cancel: error starting process";
    return;
  }
}

void ScriptPage::done (QString file)
{
  QFileInfo fi(file);

  QListWidgetItem *item = _itemList.value(fi.baseName());
  if (! item)
    return;

  _itemList.remove(fi.baseName());
  delete item;

  QStringList l;
  l << tr("Script") << fi.baseName() << tr("ended");
  g_parent->statusBar()->showMessage(l.join(" "));

  _pids.remove(fi.baseName());
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
  dialog->setIntValue(settings.value("script_launch_button_cols", 5).toInt());
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

void ScriptPage::setupScriptTimers ()
{
  ScriptDataBase db;
  QStringList l;
  db.search("*", "0", l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString file, startup, interval, command;
    if (db.load(l.at(loop), file, startup, interval, command))
      continue;

    addScriptTimer(l.at(loop), file, interval, command);
  }
}

void ScriptPage::shutDown ()
{
  // try to kill any hanging script processes
  QHashIterator<QString, qint64> it(_pids);
  while (it.hasNext())
  {
    it.next();

    QStringList args;
    args << QString::number(it.value());

    QProcess::startDetached("kill", args);
  }
}

QListWidget * ScriptPage::list ()
{
  return _queList;
}

void ScriptPage::newScriptTimer ()
{
  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("New Script Timer");

  QStringList sl;
  ScriptDataBase db;
  db.search("*", "*", sl);

  NewDialog *dialog = new NewDialog(this);
  dialog->setWindowTitle(l.join(" "));
  dialog->setTitle(tr("Enter script timer name"));
  dialog->setItems(sl);
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(editScriptTimer(QString)));
  dialog->show();
}

void ScriptPage::editScriptTimer ()
{
  SelectDialog *dialog = new SelectDialog(this);

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Select script timer to edit");
  dialog->setWindowTitle(l.join(" "));

  l.clear();
  ScriptDataBase db;
  db.search("*", "*", l);
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
  ScriptTimer *st = _timers.value(d);
  if (! st)
  {
    ScriptDataBase db;
    QString file, startup, interval, command;
    if (db.load(d, file, startup, interval, command))
      return;

    if (interval.toInt() > 0)
      addScriptTimer(d, file, interval, command);

    return;
  }

  ScriptDataBase db;
  QString file, startup, interval, command;
  if (db.load(d, file, startup, interval, command))
    return;

  if (interval == "0")
  {
    st->stop();
    _timers.remove(d);
    delete st;
    return;
  }

  st->stop();
  st->setIntervalString(interval);
  st->start();
}

void ScriptPage::deleteScriptTimer ()
{
  SelectDialog *dialog = new SelectDialog(this);

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Select script timer to delete");
  dialog->setWindowTitle(l.join(" "));

  l.clear();
  ScriptDataBase db;
  db.search("*", "*", l);
  dialog->setItems(l);

  dialog->setTitle(tr("Scripts"));
  dialog->setMode(1);
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(deleteScriptTimer2(QStringList)));
  dialog->show();
}

void ScriptPage::deleteScriptTimer2 (QStringList l)
{
  ScriptDataBase db;
  db.transaction();
  db.remove(l);
  db.commit();

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    ScriptTimer *st = _timers.value(l.at(loop));
    if (! st)
      continue;

    st->stop();
    _timers.remove(l.at(loop));
    delete st;
  }
}

void ScriptPage::runStartupScripts ()
{
  ScriptDataBase db;
  QStringList l;
  db.search("1", "*", l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QString file, startup, interval, command;
    if (db.load(l.at(loop), file, startup, interval, command))
      continue;

    runScript(l.at(loop), command);
  }
}

void ScriptPage::addScriptTimer (QString name, QString file, QString interval, QString command)
{
  ScriptTimer *st = new ScriptTimer(this);
  connect(st, SIGNAL(signalStartScript(QString, QString)), this, SLOT(runScript(QString, QString)));
  st->setName(name);
  st->setFile(file);
  st->setCommand(command);
  st->setIntervalString(interval);
  _timers.insert(name, st);
  st->start();
}
