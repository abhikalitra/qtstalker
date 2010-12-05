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

#include "GroupPage.h"
#include "Globals.h"
#include "qtstalker_defines.h"

#include "../pics/delete.xpm"
#include "../pics/delgroup.xpm"
#include "../pics/newchart.xpm"
#include "../pics/add.xpm"

#include <QStringList>
#include <QCursor>
#include <QPixmap>
#include <QLayout>
#include <QDebug>
#include <QSettings>

GroupPage::GroupPage ()
{
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(3);
  setLayout(vbox);

  _groups = new QComboBox;
  connect(_groups, SIGNAL(currentIndexChanged(int)), this, SLOT(groupSelected(int)));
  _groups->setToolTip(tr("Current Group"));
  _groups->setFocusPolicy(Qt::NoFocus);
  vbox->addWidget(_groups);

  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  loadGroups();

  QSettings settings;
  settings.beginGroup("main" + g_session);
  QString s = settings.value("last_group_used").toString();
  _groups->setCurrentIndex(_groups->findText(s, Qt::MatchExactly));

  buttonStatus();
}

void GroupPage::createActions ()
{
  QAction *action  = new QAction(QIcon(newchart_xpm), tr("&New Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("Create a new group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newGroup()));
  _actions.insert(NewGroup, action);

  action  = new QAction(QIcon(add_xpm), tr("&Add To Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Group Items") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete group items") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroupItem()));
  _actions.insert(DeleteGroupItems, action);

  action  = new QAction(QIcon(delgroup), tr("De&lete Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
  action->setToolTip(tr("Delete group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroup()));
  _actions.insert(DeleteGroup, action);
}

void GroupPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(NewGroup));
  tb->addAction(_actions.value(AddGroup));
  tb->addAction(_actions.value(DeleteGroupItems));
  tb->addAction(_actions.value(DeleteGroup));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(NewGroup));
  _menu->addAction(_actions.value(AddGroup));
  _menu->addSeparator();
  _menu->addAction(_actions.value(DeleteGroupItems));
  _menu->addAction(_actions.value(DeleteGroup));
}

void GroupPage::newGroup ()
{
  Script *script = new Script;
  prepareScript(script, "GroupPanelNewGroupDialog", "perl", "GroupPanelNewGroupDialog.pl");
}

void GroupPage::deleteGroupItem ()
{
  Script *script = new Script;
  prepareScript(script, "GroupPanelDeleteGroupItemsDialog", "perl", "GroupPanelDeleteGroupItemsDialog.pl");
}

void GroupPage::deleteGroup()
{
  Script *script = new Script;
  prepareScript(script, "GroupPanelDeleteGroupDialog", "perl", "GroupPanelDeleteGroupDialog.pl");
}

void GroupPage::groupSelected (int i)
{
  if (! _groups->count())
    return;
  
  QString s = _groups->itemText(i);

  updateList();
  
  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("last_group_used", s);
  settings.sync();
}

void GroupPage::chartOpened (BarData bd)
{
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void GroupPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void GroupPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        newGroup();
	break;
      case Qt::Key_D:
        deleteGroupItem();
	break;
      case Qt::Key_L:
        deleteGroup();
	break;
      default:
        break;
    }
  }
  else
  {
    switch(key->key())
    {
      default:
        break;
    }
  }
}

void GroupPage::loadGroups ()
{
  _groups->clear();

  QStringList cl;
  cl << "GROUP_DATABASE" << "GROUPS";

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupPage::loadGroups: no plugin" << command.plugin();
    return;
  }

  plug->command(command);
  delete plug;

  cl.clear();
  cl = command.stringData().split(",", QString::SkipEmptyParts);
  if (! cl.count())
    return;

  _groups->addItems(cl);
}

void GroupPage::updateGroups ()
{
  QString cg = _groups->currentText();
  int cr = _nav->currentRow();

  _groups->blockSignals(TRUE);
  loadGroups();
  _groups->blockSignals(FALSE);

  _groups->setCurrentIndex(_groups->findText(cg));

  _nav->blockSignals(TRUE);
  _nav->setCurrentRow(cr);
  _nav->blockSignals(FALSE);

  updateList();

  buttonStatus();
}

void GroupPage::addToGroup ()
{
  Script *script = new Script;
  prepareScript(script, "GroupPanelAddToGroupDialog", "perl", "GroupPanelAddToGroupDialog.pl");
}

void GroupPage::updateList ()
{
  _nav->clearSymbols();

  QStringList cl;
  cl << "GROUP_DATABASE" << "LOAD" << _groups->currentText();

  Command command(cl.join(","));

  ScriptPlugin *plug = _factory.plugin(command.plugin());
  if (! plug)
  {
    qDebug() << "GroupPage::updateList: no plugin" << command.plugin();
    return;
  }

  plug->command(command);
  delete plug;

  cl.clear();
  cl = command.stringData().split(",", QString::SkipEmptyParts);

  int loop = 0;
  for (; loop < cl.count(); loop++)
  {
    BarData bd;
    bd.setKey(cl.at(loop));
    _nav->addSymbol(bd);
  }

  buttonStatus();
}

void GroupPage::buttonStatus ()
{
  int status = _nav->count();
  _actions.value(DeleteGroupItems)->setEnabled(status);

  status = _groups->count();
  _actions.value(DeleteGroup)->setEnabled(status);
}

void GroupPage::prepareScript (Script *script, QString name, QString command, QString file)
{
//  connect(script, SIGNAL(signalDone(QString)), this, SLOT(done(QString)));
//  connect(this, SIGNAL(signalCancelScript(QString)), script, SLOT(stopScript(QString)));

  script->setName(name);

  QString s = INSTALL_DATA_DIR;
  s.append("/qtstalker/script/" + file);
  script->setFile(s);

  script->setCommand(command);

  script->startScript();
}
