/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2007 Stefan S. Stratigakos
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
#include "DataBase.h"
#include "Config.h"

#include "../pics/delete.xpm"
#include "../pics/delgroup.xpm"
#include "../pics/newchart.xpm"
#include "../pics/addgroup.xpm"

#include <QMessageBox>
#include <QLineEdit>
#include <QInputDialog>
#include <QStringList>
#include <QCursor>
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <stdlib.h>
#include <QToolTip>
#include <QLayout>
#include <QFileInfo>



GroupPage::GroupPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(5);
  setLayout(vbox);

  group = new QComboBox;
  connect(group, SIGNAL(currentIndexChanged(int)), this, SLOT(groupSelected(int)));
  group->setToolTip(tr("Current Group"));
  group->setFocusPolicy(Qt::NoFocus);
  vbox->addWidget(group);

  nav = new QListWidget;
  nav->setContextMenuPolicy(Qt::CustomContextMenu);
  nav->setSortingEnabled(TRUE);
  connect(nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(nav, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(nav);

  menu = new QMenu(this);
  QAction *action = menu->addAction(QIcon(newchart), tr("&New Group		Ctrl+N"), this, SLOT(newGroup()), QKeySequence(Qt::CTRL+Qt::Key_N));
  actionList.append(action);
  action = menu->addAction(QIcon(addgroup), tr("Add To &Group	Ctrl+G"), this, SLOT(addToGroup()), QKeySequence(Qt::CTRL+Qt::Key_G));
  actionList.append(action);
  action = menu->addAction(QIcon(deleteitem), tr("&Delete Group Items	Ctrl+D"), this, SLOT(deleteGroupItem()), QKeySequence(Qt::CTRL+Qt::Key_D));
  actionList.append(action);
  action = menu->addAction(QIcon(delgroup), tr("De&lete Group	Ctrl+L"), this, SLOT(deleteGroup()), QKeySequence(Qt::CTRL+Qt::Key_L));
  actionList.append(action);

  loadGroups();
  
  QString s;
  Config config;
  config.getData(Config::LastGroupUsed, s);
  group->setCurrentIndex(group->findText(s, Qt::MatchExactly));
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

  QStringList l;
  DataBase db;
  db.getAllGroupsList(l);
  if (l.contains(selection))
  {
    QMessageBox::information(this, tr("Qtstalker: Error"), tr("This group already exists."));
    return;
  }

  l.clear();
  db.setGroupList(selection, l);

  updateGroups();
}

void GroupPage::deleteGroupItem ()
{
  QListWidgetItem *item = nav->currentItem();
  if (! item)
    return;

  int rc = QMessageBox::warning(this,
		            tr("Qtstalker: Warning"),
			    tr("Are you sure you want to delete group item?"),
			    QMessageBox::Yes,
			    QMessageBox::No,
			    QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  DataBase db;
  QStringList l;
  QString s = group->currentText();
  db.getGroupList(s, l);

  l.removeOne(item->text());

  db.setGroupList(s, l);

  delete item;
}

void GroupPage::deleteGroup()
{
  QString g = group->currentText();
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

  DataBase db;
  db.deleteGroup(g);
  
  if (g == group->currentText())
    nav->clear();

  updateGroups();
}

void GroupPage::groupSelected (int i)
{
  QString s = group->itemText(i);
  if (! s.length())
    return;

  DataBase db;
  QStringList l;
  db.getGroupList(s, l);
  nav->clear();
  nav->addItems(l);

  Config config;
  config.setData(Config::LastGroupUsed, s);
}

void GroupPage::chartOpened (QListWidgetItem *item)
{
  emit fileSelected(item->text());
  emit addRecentChart(item->text());
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
  group->clear();
  QStringList l;
  DataBase db;
  db.getAllGroupsList(l);
  group->addItems(l);
}

void GroupPage::updateGroups ()
{
  int cg = group->currentIndex();
  int cr = nav->currentRow();  
  
  group->blockSignals(TRUE);
  loadGroups();
  group->blockSignals(FALSE);

  groupSelected(cg);
  
  nav->setCurrentRow(cr);
}

void GroupPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = nav->selectedItems();
  if (! sl.count())
    return;

  DataBase db;
  QStringList tl;
  db.getAllGroupsList(tl);
  bool ok;
  QString group = QInputDialog::getItem(this,
					QString(tr("Add To Group")),
					QString(tr("Select group to add selected charts")),
                                        tl,
					0,
					FALSE,
                                        &ok,
					0);
  if (! group.length())
    return;

  db.getGroupList(group, tl);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
    tl.append(sl.value(loop)->text());

  tl.removeDuplicates();
  tl.removeAll(QString());

  db.setGroupList(group, tl);

  updateGroups();
}


