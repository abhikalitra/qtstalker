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
#include "Global.h"
#include "GlobalSymbol.h"
#include "QuoteDataBase.h"
#include "SymbolDialog.h"
#include "ChartLoad.h"
#include "KeySymbol.h"
#include "GlobalSidePanel.h"

#include "../pics/add.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/delete.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QLayout>
#include <QtDebug>
#include <QSettings>

ChartPage::ChartPage (QWidget *p) : QWidget (p)
{
  setObjectName("ChartPage");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  _nav = new SymbolListWidget;
  _nav->setFocusPolicy(Qt::StrongFocus);
  _nav->setContextMenuPolicy(Qt::CustomContextMenu);
  _nav->setSelectionMode(QAbstractItemView::ExtendedSelection);
  _nav->setSortingEnabled(TRUE);
  connect(_nav, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(itemClicked(QListWidgetItem *)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  connect(_nav, SIGNAL(itemSelectionChanged()), this, SLOT(itemSelectionChanged()));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  QSettings settings(g_localSettings);
  _searchString = settings.value("last_chart_panel_symbol_search", "%").toString();

  createActions();

  createMenu();

  refresh();

  // set menu starting status
  itemSelectionChanged();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Asterisk));
  action->setToolTip(tr("Show All Symbols"));
  action->setStatusTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  _actions.insert(_SHOW_ALL, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_F));
  action->setToolTip(tr("Symbol Search") + "...");
  action->setStatusTip(tr("Symbol Search") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  _actions.insert(_SEARCH, action);

  action  = new QAction(QIcon(add_xpm), tr("Add To &Group") + "...", this);
  action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
  action->setToolTip(tr("Add symbol to group") + "...");
  action->setStatusTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(_ADD_GROUP, action);

  action  = new QAction(QIcon(delete_xpm), tr("&Delete Symbol") + "...", this);
  action->setToolTip(tr("Permanantly delete symbols from the database") + "...");
  action->setStatusTip(tr("Permanantly delete symbols from the database") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(deleteSymbol()));
  _actions.insert(_DELETE, action);

  action  = new QAction(QIcon(select_all_xpm), tr("Select All"), this);
  action->setToolTip(tr("Select All"));
  action->setStatusTip(tr("Select All"));
  connect(action, SIGNAL(activated()), _nav, SLOT(selectAll()));
  _actions.insert(_SELECT_ALL, action);

  action  = new QAction(QIcon(unselect_all_xpm), tr("Unselect All"), this);
  action->setToolTip(tr("Unselect All"));
  action->setStatusTip(tr("Unselect All"));
  connect(action, SIGNAL(activated()), _nav, SLOT(clearSelection()));
  _actions.insert(_UNSELECT_ALL, action);
}

void ChartPage::createMenu ()
{
  _menu = new QMenu(this);
  _menu->addAction(_actions.value(_SHOW_ALL));
  _menu->addAction(_actions.value(_SEARCH));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_ADD_GROUP));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_DELETE));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_SELECT_ALL));
  _menu->addAction(_actions.value(_UNSELECT_ALL));
}

void ChartPage::chartOpened (QString d)
{
  ChartLoad cl;
  cl.run(d);
}

void ChartPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void ChartPage::addToGroup ()
{
  QSettings settings(g_globalSettings);
  
  QString file = settings.value("system_script_directory").toString();
  file.append("ChartPanelAddToGroup.pl");

  g_sidePanel->scriptPanel()->runScript(QString("perl"), file);
}

void ChartPage::updateList ()
{
  QuoteDataBase db;
  QList<Symbol> l;
  db.search(_searchString, l);
  
  _nav->setSymbols(l);

  itemSelectionChanged();
}

void ChartPage::symbolSearch ()
{
  SymbolDialog *dialog = new SymbolDialog(this);
  connect(dialog, SIGNAL(signalDone(QString, QString, QStringList)), this, SLOT(setSearchDummy(QString, QString, QStringList)));
  dialog->show();
}

void ChartPage::setSearch (QString symbol)
{
  _searchString = symbol;

  QSettings settings(g_localSettings);
  settings.setValue("last_chart_panel_symbol_search", _searchString);
  settings.sync();

  updateList();
}

void ChartPage::setSearchDummy (QString symbol, QString, QStringList)
{
  setSearch(symbol);
}

void ChartPage::allButtonPressed ()
{
  _searchString = "%";

  QSettings settings(g_localSettings);
  settings.setValue("last_chart_panel_symbol_search", _searchString);
  settings.sync();

  updateList();
}

void ChartPage::selected (QStringList &l)
{
  l.clear();

  QList<QListWidgetItem *> sel = _nav->selectedItems();
  if (! sel.count())
    return;

  int loop = 0;
  for (; loop < sel.count(); loop++)
    l << sel.at(loop)->text();
}

void ChartPage::deleteSymbol ()
{
  QSettings settings(g_globalSettings);
  
  QString file = settings.value("system_script_directory").toString();
  file.append("ChartPanelRemoveSymbol.pl");

  g_sidePanel->scriptPanel()->runScript(QString("perl"), file);
}

void ChartPage::itemClicked (QListWidgetItem *d)
{
  if (! d)
    return;

  chartOpened(d->text());
}

void ChartPage::refresh ()
{
  updateList();

  KeySymbol keys;
  Data td;
  g_currentSymbol.toData(keys.indexToString(KeySymbol::_SYMBOL), td);
  
  if (td.toString().isEmpty())
    return;

  chartOpened(td.toString());
}

void ChartPage::itemSelectionChanged ()
{
  bool status = FALSE;
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (l.count())
    status = TRUE;

  _actions.value(_ADD_GROUP)->setEnabled(status);
  _actions.value(_DELETE)->setEnabled(status);
}
