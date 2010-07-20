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
#include "UpdateGroupPageThread.h"

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
  
  _nav = new ListWidget;
  _nav->setContextMenuPolicy(Qt::CustomContextMenu);
  _nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _nav->setSortingEnabled(FALSE);
  connect(_nav, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(_nav, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  connect(_nav, SIGNAL(signalEnterKeyPressed(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(_nav);

  loadGroups();

  QString s;
  Config config;
  config.getData(Config::LastGroupUsed, s);
  _groups->setCurrentIndex(_groups->findText(s, Qt::MatchExactly));

  listStatus();
}

void GroupPage::createActions ()
{
  QAction *action  = new QAction(QIcon(newchart_xpm), tr("&New Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_N));
  action->setToolTip(tr("Create a new group"));
  connect(action, SIGNAL(activated()), this, SLOT(newGroup()));
  _actions.insert(NewGroup, action);

  action  = new QAction(QIcon(addgroup), tr("&Add To Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Add symbol to group"));
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Group Items"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete group items"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroupItem()));
  _actions.insert(DeleteGroupItems, action);

  action  = new QAction(QIcon(delgroup), tr("De&lete Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_L));
  action->setToolTip(tr("Delete group"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteGroup()));
  _actions.insert(DeleteGroup, action);

  action  = new QAction(QIcon(refresh_xpm), tr("&Refresh List"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_R));
  action->setToolTip(tr("Refresh List"));
  connect(action, SIGNAL(activated()), this, SLOT(updateGroups()));
  _actions.insert(Refresh, action);
}

void GroupPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(Refresh));
  tb->addAction(_actions.value(NewGroup));
  tb->addAction(_actions.value(AddGroup));
  tb->addAction(_actions.value(DeleteGroup));
  tb->addAction(_actions.value(DeleteGroupItems));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(Refresh));
  _menu->addSeparator();
  _menu->addAction(_actions.value(NewGroup));
  _menu->addAction(_actions.value(AddGroup));
  _menu->addAction(_actions.value(DeleteGroup));
  _menu->addAction(_actions.value(DeleteGroupItems));
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
  QList<QListWidgetItem *> sl = _nav->selectedItems();
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
    if (! _group.deleteItem(_nav->row(item)))
      delete item;
  }

  GroupDataBase db;
  db.setGroup(_group);
}

void GroupPage::deleteGroup()
{
  QString g = _groups->currentText();
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

  if (g == _groups->currentText())
  {
    _nav->clear();
    _group.clear();
  }

  updateGroups();
  
  emit signalMessage(QString(tr("Group deleted.")));
}

void GroupPage::groupSelected (int i)
{
  QString s = _groups->itemText(i);
  if (! s.length())
    return;

  _group.clear();
  _group.setName(s);
  
  GroupDataBase db;
  db.getGroup(_group);
  
  updateList();
  
  Config config;
  config.setData(Config::LastGroupUsed, s);
}

void GroupPage::chartOpened (QListWidgetItem *item)
{
  if (! item)
    return;
  
  BarData bd;
  if(_group.getItem(_nav->currentRow(), bd))
    return;
  
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
  
  QStringList l;
  GroupDataBase db;
  db.getAllGroupsList(l);
  
  _groups->addItems(l);
}

void GroupPage::updateGroups ()
{
  int cg = _groups->currentIndex();
  if (cg < 0)
    cg = 0;
  int cr = _nav->currentRow();
  if (cr < 0)
    cr = 0;

  _groups->blockSignals(TRUE);
  loadGroups();
  _groups->blockSignals(FALSE);

  groupSelected(cg);

  _nav->setCurrentRow(cr);
  
  listStatus();
}

void GroupPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = _nav->selectedItems();
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
    int row = _nav->row(sl.at(loop));
    BarData bd;
    if (! _group.getItem(row, bd))
      tg.append(bd);
  }

  db.setGroup(tg);

  updateGroups();
  
  if (_group.getName() == g)
  {
    db.getGroup(_group);
    updateList();
  }
  
  emit signalMessage(QString(tr("Group item(s) added.")));
}

void GroupPage::updateList ()
{
  _nav->clear();
  
  setEnabled(FALSE);

  _nav->setSortingEnabled(FALSE);

  qRegisterMetaType<BarData>("BarData");
  UpdateGroupPageThread *r = new UpdateGroupPageThread(this, _group);
  _group.clear();
  connect(r, SIGNAL(signalSymbol(BarData)), this, SLOT(addSymbol(BarData)), Qt::QueuedConnection);
  connect(r, SIGNAL(signalDone()), this, SLOT(requestDone()), Qt::QueuedConnection);
  connect(r, SIGNAL(finished()), r, SLOT(deleteLater()));
  r->start();
}

void GroupPage::addSymbol (BarData bd)
{
  _group.append(bd);

  QListWidgetItem *item = new QListWidgetItem;
  item->setText(bd.getSymbol());
  item->setToolTip(QString(tr("Name: ") + bd.getName() + "\n" + tr("Exchange: ") + bd.getExchange()));
  _nav->addItem(item);
}

void GroupPage::requestDone ()
{
  _nav->setSortingEnabled(TRUE);
  setEnabled(TRUE);
  listStatus();
}

void GroupPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(DeleteGroupItems)->setEnabled(status);
  
  if (_groups->count())
  {
    _actions.value(DeleteGroup)->setEnabled(TRUE);
    _actions.value(AddGroup)->setEnabled(TRUE);
  }
  else
  {
    _actions.value(DeleteGroup)->setEnabled(FALSE);
    _actions.value(AddGroup)->setEnabled(FALSE);
  }
    
}




