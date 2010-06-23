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
#include "DBPluginFactory.h"
#include "Config.h"
#include "SymbolDialog.h"
#include "QuoteIndexDataBase.h"

#include "../pics/addgroup.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/delete.xpm"

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
  
  _nav = new ListWidget;
  _nav->setContextMenuPolicy(Qt::CustomContextMenu);
  _nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _nav->setSortingEnabled(FALSE);
  connect(_nav, SIGNAL(itemSelectionChanged()), this, SLOT(listStatus()));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(_nav, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  connect(_nav, SIGNAL(signalEnterKeyPressed(QListWidgetItem *)), this, SLOT(chartOpened(QListWidgetItem *)));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  Config config;
  config.getData(Config::LastChartPanelExchangeSearch, _searchExchange);
  config.getData(Config::LastChartPanelSymbolSearch, _searchString);

  updateList();
  listStatus();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_A));
  action->setToolTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  _actions.insert(ShowAll, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_S));
  action->setToolTip(tr("Symbol Search"));
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  _actions.insert(Search, action);

  action  = new QAction(QIcon(addgroup), tr("Add To &Group"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
  action->setToolTip(tr("Add symbol to group"));
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Symbol"), this);
  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_D));
  action->setToolTip(tr("Delete symbol from the database permanently"));
  connect(action, SIGNAL(activated()), this, SLOT(deleteSymbol()));
  _actions.insert(Delete, action);
}

void ChartPage::createButtonMenu (QToolBar *tb)
{
  tb->addAction(_actions.value(ShowAll));
  tb->addAction(_actions.value(Search));
  tb->addAction(_actions.value(AddGroup));
  tb->addAction(_actions.value(Delete));

  _menu = new QMenu(this);
  _menu->addAction(_actions.value(AddGroup));
  _menu->addSeparator();
  _menu->addAction(_actions.value(ShowAll));
  _menu->addAction(_actions.value(Search));
  _menu->addSeparator();
  _menu->addAction(_actions.value(Delete));
}

void ChartPage::chartOpened (QListWidgetItem *item)
{
  if (! item)
    return;

  BarData bd;
  if (_symbols.getItem(_nav->currentRow(), bd))
    return;
  
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void ChartPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void ChartPage::addToGroup ()
{
  QList<QListWidgetItem *> sl = _nav->selectedItems();
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
    BarData bd;
    if (_symbols.getItem(_nav->row(sl.at(loop)), bd))
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
  _nav->clear();
  _symbols.clear();

  QuoteIndexDataBase idb;
  idb.getSearchList(_searchExchange, _searchString, _symbols);

  DBPluginFactory fac;

  int loop;
  for (loop = 0; loop < _symbols.count(); loop++)
  {
    BarData bd;
    _symbols.getItem(loop, bd);

    // get the name field
    QString name;
    DBPlugin *plug = fac.plugin(bd.getPlugin());
    if (plug)
    {
      QString s = "NAME";
      plug->detail(s, &bd, name);
    }
    
    QListWidgetItem *item = new QListWidgetItem;
    item->setText(bd.getSymbol());
    item->setToolTip(QString(tr("Name: ") + name + "\n" + tr("Exchange: ") + bd.getExchange()));
    _nav->addItem(item);
  }
}

void ChartPage::symbolSearch ()
{
  SymbolDialog dialog(1);
  dialog.setSymbols(_searchExchange, _searchString);
  int rc = dialog.exec();
  if (rc == QDialog::Rejected)
    return;

  dialog.getSymbolSearch(_searchExchange, _searchString);
  
  Config config;
  config.setData(Config::LastChartPanelSymbolSearch, _searchString);
  config.setData(Config::LastChartPanelExchangeSearch, _searchExchange);
  
  updateList();
}

void ChartPage::allButtonPressed ()
{
  _searchExchange.clear();
  _searchString.clear();
  updateList();
}

void ChartPage::listStatus ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (l.count())
    status = TRUE;
  
  _actions.value(AddGroup)->setEnabled(status); 
}

void ChartPage::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  int rc = QMessageBox::warning(this,
                                tr("Qtstalker: Warning"),
                                tr("Are you sure you want to permanently delete symbol(s)?"),
                                QMessageBox::Yes,
                                QMessageBox::No,
                                QMessageBox::NoButton);

  if (rc == QMessageBox::No)
    return;

  DBPluginFactory fac;

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QListWidgetItem *item = l.at(loop);
    int row = _nav->row(item);
    BarData bd;
    if (_symbols.getItem(row, bd))
    {
      qDebug() << "ChartPage::deleteSymbol: BarData not found" << row;
      continue;
    }

    DBPlugin *plug = fac.plugin(bd.getPlugin());
    if (! plug)
    {
      qDebug() << "ChartPage::deleteSymbol: plugin not found" << bd.getPlugin();
      continue;
    }

    if (plug->deleteSymbol(&bd))
    {
      qDebug() << "ChartPage::deleteSymbol: plugin deleteSymbol error";
      continue;
    }
  }

  updateList();
}

