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
#include "qtstalker_defines.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/newchart.xpm"
//#include "../pics/ok.xpm"
#include "../pics/script.xpm"
#include "../pics/configure.xpm"

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

ScriptPage::ScriptPage ()
{
  createActions();

  createGUI();

  queStatus();
}

void ScriptPage::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);
  
  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  // create button grid
  QGridLayout *grid = new QGridLayout;
  grid->setMargin(0);
  grid->setSpacing(0);
//  grid->setColumnStretch(5, 1);
  vbox->addLayout(grid);

  QSettings settings;
  settings.beginGroup("main");
  int rows = settings.value("script_launch_button_rows", 2).toInt();
  int cols = settings.value("script_launch_button_cols", 4).toInt();

  int loop = 0;
  int pos = 1;
  for (; loop < rows; loop++)
  {
    int loop2 = 0;
    for (; loop2 < cols; loop2++, pos++)
    {
      ScriptLaunchButton *b = new ScriptLaunchButton(pos, pos);
      connect(b, SIGNAL(signalButtonClicked(QString)), this, SLOT(runScript(QString)));
      grid->addWidget(b, loop, loop2);
    }
  }

  vbox->addSpacing(5);
  
  QLabel *label = new QLabel(tr("Active Scripts"));
  vbox->addWidget(label);
  
  _queList = new QListWidget;
  _queList->setContextMenuPolicy(Qt::CustomContextMenu);
  _queList->setSortingEnabled(TRUE);
  connect(_queList, SIGNAL(itemSelectionChanged()), this, SLOT(queStatus()));
  connect(_queList, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(queRightClick(const QPoint &)));
  vbox->addWidget(_queList);
}

void ScriptPage::createActions ()
{
  QAction *action = new QAction(QIcon(newchart_xpm), tr("&New Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("New Script"));
  connect(action, SIGNAL(activated()), this, SLOT(newScript()));
  _actions.insert(_NewScript, action);

  action = new QAction(QIcon(edit_xpm), tr("&Edit Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  action->setToolTip(tr("Edit Script"));
  connect(action, SIGNAL(activated()), this, SLOT(editScript()));
  _actions.insert(_EditScript, action);

  action = new QAction(QIcon(delete_xpm), tr("&Delete Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete Script"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteScript()));
  _actions.insert(_DeleteScript, action);

  action = new QAction(QIcon(delete_xpm), tr("&Cancel Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_C));
  action->setToolTip(tr("Cancel script"));
  connect(action, SIGNAL(activated()), this, SLOT(cancelScript()));
  _actions.insert(_CancelScript, action);

  action = new QAction(QIcon(script_xpm), tr("&Run Script"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Run script"));
  connect(action, SIGNAL(activated()), this, SLOT(runScript()));
  _actions.insert(_RunScript, action);

  action = new QAction(QIcon(configure_xpm), tr("Launch Button Rows"), this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Launch Button Rows"));
  connect(action, SIGNAL(activated()), this, SLOT(launchButtonRows()));
  _actions.insert(_LaunchButtonRows, action);

  action = new QAction(QIcon(configure_xpm), tr("Launch Button Columns"), this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Launch Button Columns"));
  connect(action, SIGNAL(activated()), this, SLOT(launchButtonCols()));
  _actions.insert(_LaunchButtonCols, action);
}

void ScriptPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(_RunScript));
  tb->addAction(_actions.value(_NewScript));
  tb->addAction(_actions.value(_EditScript));
  tb->addAction(_actions.value(_DeleteScript));

  _queMenu = new QMenu(this);
  _queMenu->addAction(_actions.value(_RunScript));
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
  Script *script = new Script;
  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/script/ScriptPanelNewScript.pl");
  prepareScript(script, "ScriptPanelNewScript", "perl", s);
}

void ScriptPage::editScript ()
{
  Script *script = new Script;
  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/script/ScriptPanelEditScript.pl");
  prepareScript(script, "ScriptPanelEditScript", "perl", s);
}

void ScriptPage::deleteScript ()
{
  Script *script = new Script;
  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/script/ScriptPanelDeleteScript.pl");
  prepareScript(script, "ScriptPanelDeleteScript", "perl", s);
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

void ScriptPage::loadSettings ()
{
//  QSettings settings;
//  settings.beginGroup("main"); // global setting

}

void ScriptPage::saveSettings ()
{
//  QSettings settings;
//  settings.beginGroup("main"); // global setting
//  settings.sync();
}

void ScriptPage::runScript ()
{
  Script *script = new Script;
  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/script/ScriptPanelRunScript.pl");
  prepareScript(script, "ScriptPanelRunScript", "perl", s);
}

void ScriptPage::runScript (QString d)
{
  QStringList cl;
  cl << "SCRIPT_DATABASE" << "LOAD" << d;

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "ScriptPage::runScript: no plugin" << command.plugin();
    return;
  }

  plug->command(&command);
  delete plug;

  cl = command.stringData().split(",", QString::SkipEmptyParts);
  if (cl.count() != 2)
    return;

  QListWidgetItem *item = new QListWidgetItem(_queList);
  item->setText(d);
  _itemList.insert(d, item);

  Script *script = new Script;
  prepareScript(script, d, cl.at(1), cl.at(0));
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
}

void ScriptPage::runFileScript ()
{
  Script *script = new Script;
  prepareScript(script,
                "testrunscript",
                "perl",
                "/media/data/mystuff/prog/qtstalker-test/misc/CUS_examples/testrunscript.perl");
}

void ScriptPage::prepareScript (Script *script, QString name, QString command, QString file)
{
  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
  connect(this, SIGNAL(signalCancelScript(QString)), script, SLOT(stopScript(QString)));
//  connect(script, SIGNAL(signalMessage(QString)), _log, SLOT(append(const QString &)));

  script->setName(name);
  script->setFile(file);
  script->setCommand(command);
  script->startScript();
}

void ScriptPage::launchButtonRows ()
{
  QSettings settings;
  settings.beginGroup("main");
  
  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_rows", 2).toInt());
  dialog->setLabelText(tr("Rows"));
  dialog->setWindowTitle(tr("Configure Launch Button Rows"));
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(launchButtonRows2(int)));
  dialog->show();
}

void ScriptPage::launchButtonRows2 (int d)
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("script_launch_button_rows", d);
  settings.sync();
}

void ScriptPage::launchButtonCols ()
{
  QSettings settings;
  settings.beginGroup("main");

  QInputDialog *dialog = new QInputDialog(this);
  dialog->setIntValue(settings.value("script_launch_button_cols", 4).toInt());
  dialog->setLabelText(tr("Columns"));
  dialog->setWindowTitle(tr("Configure Launch Button Columns"));
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(launchButtonCols2(int)));
  dialog->show();
}

void ScriptPage::launchButtonCols2 (int d)
{
  QSettings settings;
  settings.beginGroup("main");
  settings.setValue("script_launch_button_cols", d);
  settings.sync();
}
