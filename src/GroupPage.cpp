/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2011 Stefan S. Stratigakos
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
#include "GroupDataBase.h"
#include "QuoteDataBase.h"
#include "GroupEditDialog.h"
#include "SelectDialog.h"
#include "NewDialog.h"

#include "../pics/edit.xpm"
#include "../pics/delete.xpm"
#include "../pics/new.xpm"
#include "../pics/add.xpm"

#include <QStringList>
#include <QCursor>
#include <QPixmap>
#include <QLayout>
#include <QDebug>
#include <QSettings>

GroupPage::GroupPage (QWidget *p) : QWidget (p)
{
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(3);
  setLayout(vbox);

  createButtonMenu();
  
  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  _groups = new QComboBox;
  connect(_groups, SIGNAL(currentIndexChanged(int)), this, SLOT(groupSelected(int)));
  _groups->setToolTip(tr("Current Group"));
  _groups->setFocusPolicy(Qt::NoFocus);
  vbox->addWidget(_groups);

  loadGroups();

  QSettings settings(g_localSettings);
  QString s = settings.value("last_group_used").toString();
  _groups->setCurrentIndex(_groups->findText(s, Qt::MatchExactly));
}

void GroupPage::createActions ()
{
  QAction *action  = new QAction(QIcon(new_xpm), tr("&New Group") + "...", this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("Create a new group") + "...");
  action->setStatusTip(tr("Create a new group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(newGroup()));
  _actions.insert(_NEW_GROUP, action);

  action  = new QAction(QIcon(add_xpm), tr("&Add To Group") + "...", this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Add symbol to group") + "...");
  action->setStatusTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(_ADD_GROUP, action);

  action  = new QAction(QIcon(edit_xpm), tr("&Edit Group Items") + "...", this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_E));
  action->setToolTip(tr("Edit group") + "...");
  action->setStatusTip(tr("Edit group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(editGroup()));
  _actions.insert(_EDIT_GROUP, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Group") + "...", this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete group") + "...");
  action->setStatusTip(tr("Delete group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroup()));
  _actions.insert(_DELETE_GROUP, action);
}

void GroupPage::createButtonMenu ()
{
  _menu = new QMenu(this);
  _menu->addAction(_actions.value(_NEW_GROUP));
  _menu->addAction(_actions.value(_ADD_GROUP));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_EDIT_GROUP));
  _menu->addAction(_actions.value(_DELETE_GROUP));
}

void GroupPage::newGroup ()
{
  GroupDataBase db;
  QStringList l;
  db.groups(l);
  
  NewDialog *dialog = new NewDialog(this);
  dialog->setItems(l);
  dialog->setTitle(tr("Enter new group name"));
  connect(dialog, SIGNAL(signalDone(QString)), this, SLOT(editDialog(QString)));

  l.clear();
  l << "QtStalker" + g_session + ":" << tr("New Group");
  dialog->setWindowTitle(l.join(" "));
  dialog->show();
}
  
void GroupPage::editDialog (QString d)
{
  GroupEditDialog *dialog = new GroupEditDialog(this, d);
  connect(dialog, SIGNAL(accepted()), this, SLOT(updateGroups()));
  dialog->show();
}

void GroupPage::editGroup ()
{
  editDialog(_groups->currentText());
}

void GroupPage::deleteGroup ()
{
  SelectDialog *dialog = new SelectDialog(this);

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Delete Group");
  dialog->setWindowTitle(l.join(" "));

  GroupDataBase db;
  db.groups(l);
  dialog->setItems(l);
  
  dialog->setTitle(tr("Groups"));
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(deleteGroup2(QStringList)));
  dialog->show();
}

void GroupPage::deleteGroup2 (QStringList l)
{
  GroupDataBase db;
  db.deleteGroup(l);
  updateGroups();
}

void GroupPage::groupSelected (int i)
{
  if (! _groups->count())
    return;
  
  QString s = _groups->itemText(i);

  updateList();
  
  QSettings settings(g_localSettings);
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

void GroupPage::loadGroups ()
{
  _groups->clear();

  GroupDataBase db;
  QStringList l;
  db.groups(l);

  if (! l.count())
    return;

  _groups->addItems(l);
}

void GroupPage::updateGroups ()
{
  QString cg = _groups->currentText();
  int cr = _nav->currentRow();

  _groups->blockSignals(TRUE);
  loadGroups();
  _groups->blockSignals(FALSE);

  if (! cg.isEmpty())
    _groups->setCurrentIndex(_groups->findText(cg));

  _nav->blockSignals(TRUE);
  if (cr > -1)
    _nav->setCurrentRow(cr);
  _nav->blockSignals(FALSE);

  updateList();
}

void GroupPage::addToGroup ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  GroupDataBase db;
  QStringList l2;
  db.groups(l2);

  SelectDialog *dialog = new SelectDialog(this);
  dialog->setItems(l2);
  dialog->setTitle(tr("Groups"));
  dialog->setMode(1);
  connect(dialog, SIGNAL(signalDone(QStringList)), this, SLOT(addToGroup2(QStringList)));

  l2.clear();
  l2 << "QtStalker" + g_session + ":" << tr("Add To Group");
  dialog->setWindowTitle(l2.join(" "));

  dialog->show();
}

void GroupPage::addToGroup2 (QStringList gl)
{
  QList<QListWidgetItem *> l = _nav->selectedItems();

  QStringList l2;
  int loop = 0;
  for (; loop < l.count(); loop++)
    l2 << l.at(loop)->text();

  GroupDataBase db;
  db.merge(gl.at(0), l2);

  updateList();
}

void GroupPage::updateList ()
{
  _nav->clearSymbols();

  if (! _groups->count())
    return;

  GroupDataBase db;
  QStringList l;
  db.load(_groups->currentText(), l);

  QuoteDataBase qdb;
  
  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    BarData bd;
    bd.setKey(l.at(loop));
    qdb.getSymbol(&bd);
    _nav->addSymbol(bd);
  }
}

SymbolListWidget * GroupPage::list ()
{
  return _nav;
}
