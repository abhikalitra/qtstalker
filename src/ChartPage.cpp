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
#include "Globals.h"
#include "QuoteDataBase.h"

#include "../pics/add.xpm"
#include "../pics/search.xpm"
#include "../pics/asterisk.xpm"
#include "../pics/delete.xpm"
#include "../pics/export.xpm"
#include "../pics/select_all.xpm"
#include "../pics/unselect_all.xpm"

#include <QCursor>
#include <QToolTip>
#include <QLayout>
#include <QMenu>
#include <QKeyEvent>
#include <QtDebug>
#include <QList>
#include <QSettings>

ChartPage::ChartPage (QWidget *p) : QWidget (p)
{
  setObjectName("ChartPage");
  
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  QSettings settings(g_localSettings);
  _searchExchange = settings.value("last_chart_panel_exchange_search", "*").toString();
  _searchString = settings.value("last_chart_panel_symbol_search", "*").toString();

  createActions();

  createMenu();

  updateList();
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

  action  = new QAction(QIcon(export_xpm), tr("E&xport Symbol") + "...", this);
  action->setToolTip(tr("Export symbols to CSV files") + "...");
  action->setStatusTip(tr("Export symbols to CSV files") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(exportSymbol()));
  _actions.insert(_EXPORT, action);

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
  _menu->addAction(_actions.value(_EXPORT));
  _menu->addSeparator();
  _menu->addAction(_actions.value(_SELECT_ALL));
  _menu->addAction(_actions.value(_UNSELECT_ALL));
}

void ChartPage::chartOpened (BarData bd)
{
  emit fileSelected(bd);
  emit addRecentChart(bd);
}

void ChartPage::rightClick (const QPoint &)
{
  _menu->exec(QCursor::pos());
}

void ChartPage::addToGroup ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  QStringList l2;
  int loop = 0;
  for (; loop < l.count(); loop++)
    l2 << l.at(loop)->text();

  QSettings settings(g_localSettings);
  settings.setValue("chart_panel_selected", l2.join(";"));
  settings.sync();
  
  QSettings settings2(g_globalSettings);
  Script *script = new Script(this);
  script->setName("ChartPanelAddGroup");
  script->setFile(settings2.value("chart_panel_add_group_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ChartPage::updateList ()
{
  _nav->clearSymbols();

  _nav->setSortingEnabled(FALSE);

  BarData bd;
  bd.setExchange(_searchExchange);
  bd.setSymbol(_searchString);
  
  QuoteDataBase db;
  QList<BarData> l;
  db.search(&bd, l);

  int loop = 0;
  for (; loop < l.count(); loop++)
    _nav->addSymbol(l.at(loop));

  _nav->setSortingEnabled(TRUE);
  
  buttonStatus();
}

void ChartPage::symbolSearch ()
{
  QSettings settings(g_globalSettings);
  Script *script = new Script(this);
  script->setName("ChartPanelSymbolSearch");
  script->setFile(settings.value("chart_panel_symbol_search_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ChartPage::setSearch (QString exchange, QString symbol)
{
  _searchExchange = exchange;
  _searchString = symbol;
  
  QSettings settings(g_localSettings);
  settings.setValue("last_chart_panel_symbol_search", _searchString);
  settings.setValue("last_chart_panel_exchange_search", _searchExchange);
  settings.sync();
  
  updateList();
}

void ChartPage::allButtonPressed ()
{
  _searchExchange = "*";
  _searchString = "*";

  QSettings settings(g_localSettings);
  settings.setValue("last_chart_panel_symbol_search", _searchString);
  settings.setValue("last_chart_panel_exchange_search", _searchExchange);
  settings.sync();
  
  updateList();
}

void ChartPage::buttonStatus ()
{
  int status = _nav->count();
  _actions.value(_ADD_GROUP)->setEnabled(status);
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

SymbolListWidget * ChartPage::list ()
{
  return _nav;
}

void ChartPage::deleteSymbol ()
{
  QList<QListWidgetItem *> l = _nav->selectedItems();
  if (! l.count())
    return;

  QStringList l2;
  int loop = 0;
  for (; loop < l.count(); loop++)
    l2 << l.at(loop)->text();

  QSettings settings(g_localSettings);
  settings.setValue("chart_panel_selected", l2.join(";"));
  settings.sync();

  QSettings settings2(g_globalSettings);
  Script *script = new Script(this);
  script->setName("ChartPanelDelete");
  script->setFile(settings2.value("chart_panel_delete_script").toString());
  script->setCommand("perl");
  script->startScript();
}

void ChartPage::exportSymbol ()
{
  QSettings settings(g_globalSettings);
  Script *script = new Script(this);
  script->setName("ChartPanelSymbolExport");
  script->setFile(settings.value("chart_panel_symbol_export_script").toString());
  script->setCommand("perl");
  script->startScript();
}
