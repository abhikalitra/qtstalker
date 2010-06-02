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
#include "GroupDataBase.h"
#include "Config.h"

#include "../pics/delete.xpm"
#include "../pics/delgroup.xpm"
#include "../pics/newchart.xpm"
#include "../pics/addgroup.xpm"
#include "../pics/refresh.xpm"

#include <QMessageBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QStringList>
#include <QCursor>
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QToolTip>
#include <QLayout>
#include <QFileInfo>

GroupPage::GroupPage (QWidget *w) : QWidget (w)
{
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(3);
  setLayout(vbox);

  groups = new QComboBox;
  connect(groups, SIGNAL(currentIndexChanged(int)), this, SLOT(groupSelected(int)));
  groups->setToolTip(tr("Current Group"));
  groups->setFocusPolicy(Qt::NoFocus);
  vbox->addWidget(groups);

  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  nav = new ListWidget;
  nav->setContextMenuPolicy(Qt::CustomContextMenu);
  nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  nav->setSortingEnabled(FALSE);
  connect(nav, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(nav, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  connect(nav, SIGNAL(signalEnterKeyPressed(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(nav);

  loadGroups();

  QString s;
  Config config;
  config.getData(Config::LastGroupUsed, s);
  groups->setCurrentIndex(groups->findText(s, Qt::MatchExactly));

  listStatus();
}

void GroupPage::createActions ()
{
  QAction *action  = new QAction(QIcon(newchart_xpm), tr("&New Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("Create a new group"));
  connect(action, SIGNAL(activated()), this, SLOT(newGroup()));
  actions.insert(NewGroup, action);

  action  = new QAction(QIcon(addgroup), tr("&Add To Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Add symbol to group"));
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  actions.insert(AddGroup, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Group Items"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete group items"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroupItem()));
  actions.insert(DeleteGroupItems, action);

  action  = new QAction(QIcon(delgroup), tr("De&lete Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
  action->setToolTip(tr("Delete group"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroup()));
  actions.insert(DeleteGroup, action);

  action  = new QAction(QIcon(refresh_xpm), tr("&Refresh List"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Refresh List"));
  connect(action, SIGNAL(activated()), this, SLOT(updateGroups()));
  actions.insert(Refresh, action);
}

void GroupPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(actions.value(Refresh));
  tb->addAction(actions.value(NewGroup));
  tb->addAction(actions.value(AddGroup));
  tb->addAction(actions.value(DeleteGroup));
  tb->addAction(actions.value(DeleteGroupItems));

  menu = new QMenu(this);
  menu->addAction(actions.value(Refresh));
  menu->addSeparator();
  menu->addAction(actions.value(NewGroup));
  menu->addAction(actions.value(AddGroup));
  menu->addAction(actions.value(DeleteGroup));
  menu->addAction(actions.value(DeleteGroupItems));
}

void GroupPage::newGroup()
{
  bool ok;
  QString selection = QInputDialog::getText(this,
					    tr("New Group"),
					    tr("Enter new group symbol."),
					    QLineEdit::Normal,
					    tr("NewGroup"),
					    &ok);
  if (selection.isEmpty())
    return;
  
  // remove any forbidden sql characters
  selection = selection.remove(QString("'"), Qt::CaseSensitive);

  QStringList l;
  GroupDataBase db;
  db.getAllGroupsList(l);
  if (l.contains(selection))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  Group g;
  g.setName(selection);
  db.setGroup(g);

  updateGroups();
  
  emit signalMessage(QString(tr("Group created.")));
}

void GroupPage::deleteGroupItem ()
{
  QList<QListWidgetItem *> sl = nav->selectedItems();
  if (! sl.count())
    return;

  int rc = QMessageBox::warning(this,
				tr("Qtstalker: Warning"),
				tr("Are you sure you want to delete group items?"),
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  int loop;
  for (loop = sl.count() - 1; loop > -1; loop--)
  {
    QListWidgetItem *item = sl.at(loop);
    if (! group.deleteItem(nav->row(item)))
      delete item;
  }

  GroupDataBase db;
  db.setGroup(group);
}

void GroupPage::deleteGroup()
{
  QString g = groups->currentText();
  if (! g.length())
    return;

  QString s = tr("Are you sure you want to delete current group ") + g;
  int rc = QMessageBox::warning(this,
  				tr("Qtstalker: Warning"),
				s,
				QMessageBox::Yes,
				QMessageBox::No,
				QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  GroupDataBase db;
  db.deleteGroup(g);

  if (g == groups->currentText())
  {
    nav->clear();
    group.clear();
  }

  updateGroups();
  
  emit signalMessage(QString(tr("Group deleted.")));
}

void GroupPage::groupSelected (int i)
{
  QString s = groups->itemText(i);
  if (! s.length())
    return;

  group.clear();
  group.setName(s);
  
  GroupDataBase db;
  db.getGroup(group);
  
  updateList();
  
  Config config;
  config.setData(Config::LastGroupUsed, s);
}

void GroupPage::chartOpened (QListWidgetItem *item)
{
  if (! item)
    return;
  
  BarData *bd = group.getItem(nav->currentRow());
  if (! bd)
    return;
  
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void GroupPage::rightClick (const QPoint &)
{
  menu->exec(QCursor::pos());
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
  groups->clear();
  
  QStringList l;
  GroupDataBase db;
  db.getAllGroupsList(l);
  
  groups->addItems(l);
}

void GroupPage::updateGroups ()
{
  int cg = groups->currentIndex();
  if (cg < 0)
    cg = 0;
  int cr = nav->currentRow();
  if (cr < 0)
    cr = 0;

  groups->blockSignals(TRUE);
  loadGroups();
  groups->blockSignals(FALSE);

  groupSelected(cg);

  nav->setCurrentRow(cr);
  
  listStatus();
}

void GroupPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = nav->selectedItems();
  if (! sl.count())
    return;

  GroupDataBase db;
  QStringList tl;
  db.getAllGroupsList(tl);
  bool ok;
  QString g = QInputDialog::getItem(this,
				    QString(tr("Add To Group")),
				    QString(tr("Select group to add selected charts")),
                                    tl,
				    0,
				    FALSE,
                                    &ok,
				    0);
  if (! ok || ! g.length())
    return;

  Group tg;
  tg.setName(g);
  db.getGroup(tg);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
  {
    int row = nav->row(sl.at(loop));
    BarData *bd = group.getItem(row);
    if (bd)
    {
      BarData *bd2 = new BarData;
      bd2->setExchange(bd->getExchange());
      bd2->setSymbol(bd->getSymbol());
      bd2->setName(bd->getName());
      tg.append(bd2);
    }
  }

  db.setGroup(tg);

  updateGroups();
  
  if (group.getName() == g)
  {
    db.getGroup(group);
    updateList();
  }
  
  emit signalMessage(QString(tr("Group item(s) added.")));
}

void GroupPage::updateList ()
{
  nav->clear();
  
  int loop;
  for (loop = 0; loop < group.count(); loop++)
  {
    BarData *bd = group.getItem(loop);
    if (! bd)
      continue;
    
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(bd->getSymbol());
    item->setToolTip(QString(tr("Name: ") + bd->getName() + "\n" + tr("Exchange: ") + bd->getExchange()));
    nav->addItem(item);
  }
}

void GroupPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = nav->selectedItems();
  if (l.count())
    status = TRUE;
  
  actions.value(AddGroup)->setEnabled(status); 
  actions.value(DeleteGroupItems)->setEnabled(status);
  
  if (groups->count())
    actions.value(DeleteGroup)->setEnabled(TRUE); 
  else
    actions.value(DeleteGroup)->setEnabled(FALSE); 
}




