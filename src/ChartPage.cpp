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

#include "ChartPage.h"
#include "GroupDataBase.h"
#include "DBPlugin.h"
#include "Config.h"
#include "SymbolDialog.h"

#include "../pics/addgroup.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"

#include <QMessageBox>
#include <QCursor>
#include <QToolTip>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QtDebug>
#include <QInputDialog>
#include <QGridLayout>
#include <QList>

ChartPage::ChartPage (QWidget *w) : QWidget (w)
{
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  QToolBar *tb = new QToolBar;
  vbox->addWidget(tb);
  tb->setIconSize(QSize(18, 18));
  
  createButtonMenu(tb);
  
  nav = new QListWidget;
  nav->setContextMenuPolicy(Qt::CustomContextMenu);
  nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  nav->setSortingEnabled(FALSE);
  connect(nav, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(nav, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(nav);

  // update to last symbol search before displaying
  Config config;
  config.getData(Config::LastChartPanelExchangeSearch, searchExchange);
  config.getData(Config::LastChartPanelSymbolSearch, searchString);

  updateList();
  listStatus();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  actions.insert(ShowAll, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Symbol Search"));
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  actions.insert(Search, action);

  action  = new QAction(QIcon(addgroup), tr("Add To &Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
  action->setToolTip(tr("Add symbol to group"));
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  actions.insert(AddGroup, action);
}

void ChartPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(actions.value(ShowAll));
  tb->addAction(actions.value(Search));
  tb->addAction(actions.value(AddGroup));

  menu = new QMenu(this);
  menu->addAction(actions.value(AddGroup));
  menu->addSeparator();
  menu->addAction(actions.value(ShowAll));
  menu->addAction(actions.value(Search));
}

void ChartPage::chartOpened (QListWidgetItem *item)
{
  if (! item)
    return;

  BarData *bd = symbols.getItem(nav->currentRow());
  if (! bd)
    return;
  
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void ChartPage::rightClick (const QPoint &)
{
  menu->exec(QCursor::pos());
}

void ChartPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = nav->selectedItems();
  if (! sl.count())
    return;

  GroupDataBase db;
  QStringList tl;
  db.getAllGroupsList(tl);
  bool ok;
  QString name = QInputDialog::getItem(this,
					QString(tr("Add To Group")),
					QString(tr("Select group to add selected charts")),
                                        tl,
					0,
					FALSE,
                                        &ok,
					0);
  if (! ok || ! name.length())
    return;

  Group group;
  group.setName(name);
  db.getGroup(group);

  int loop;
  for (loop = 0; loop < sl.count(); loop++)
  {
    BarData *bd = symbols.getItem(nav->row(sl.at(loop)));
    if (! bd)
      continue;
    
    group.append(bd);
  }

  db.setGroup(group);

  emit signalAddToGroup();
  emit signalMessage(QString(tr("Symbol(s) added.")));
}

void ChartPage::doKeyPress (QKeyEvent *key)
{
  key->accept();

  if (key->modifiers() == Qt::ControlModifier)
  {
    switch(key->key())
    {
      case Qt::Key_G:
        addToGroup();
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

void ChartPage::updateList ()
{
  nav->clear();
  symbols.clear();

  DBPlugin db;
  db.getSearchList(searchExchange, searchString, symbols);

  int loop;
  for (loop = 0; loop < symbols.count(); loop++)
  {
    BarData *bd = symbols.getItem(loop);
    
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(bd->getSymbol());
    item->setToolTip(QString(tr("Name: ") + bd->getName() + "\n" + tr("Exchange: ") + bd->getExchange()));
    nav->addItem(item);
  }
}

void ChartPage::symbolSearch ()
{
  SymbolDialog dialog(1);
  dialog.setSymbols(searchExchange, searchString);
  int rc = dialog.exec();
  if (rc == QDialog::Rejected)
    return;

  dialog.getSymbolSearch(searchExchange, searchString);
  
  Config config;
  config.setData(Config::LastChartPanelSymbolSearch, searchString);
  config.setData(Config::LastChartPanelExchangeSearch, searchExchange);
  
  updateList();
}

void ChartPage::allButtonPressed ()
{
  searchExchange.clear();
  searchString.clear();
  updateList();
}

void ChartPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = nav->selectedItems();
  if (l.count())
    status = TRUE;
  
  actions.value(AddGroup)->setEnabled(status); 
}

