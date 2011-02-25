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
  
  createActions();

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(0);
  vbox->setSpacing(0);
  setLayout(vbox);

  createButtonMenu();
  
  _nav = new SymbolListWidget;
  connect(_nav, SIGNAL(signalSymbolSelected(BarData)), this, SLOT(chartOpened(BarData)));
  connect(_nav, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(rightClick(const QPoint &)));
  vbox->addWidget(_nav);

  // update to last symbol search before displaying
  QSettings settings(g_localSettings);
  _searchExchange = settings.value("last_chart_panel_exchange_search", "*").toString();
  _searchString = settings.value("last_chart_panel_symbol_search", "*").toString();

  updateList();
}

void ChartPage::createActions ()
{
  QAction *action  = new QAction(QIcon(asterisk_xpm), tr("Show &All Symbols"), this);
//  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_A));
  action->setToolTip(tr("Show All Symbols"));
  action->setStatusTip(tr("Show All Symbols"));
  connect(action, SIGNAL(activated()), this, SLOT(allButtonPressed()));
  _actions.insert(ShowAll, action);

  action  = new QAction(QIcon(search_xpm), tr("Symbol &Search") + "...", this);
//  action->setShortcut(QKeySequence(Qt::ALT+Qt::Key_S));
  action->setToolTip(tr("Symbol Search") + "...");
  action->setStatusTip(tr("Symbol Search") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(symbolSearch()));
  _actions.insert(Search, action);

  action  = new QAction(QIcon(add_xpm), tr("Add To &Group") + "...", this);
//  action->setShortcut(QKeySequence(Qt::CTRL+Qt::Key_G));
  action->setToolTip(tr("Add symbol to group") + "...");
  action->setStatusTip(tr("Add symbol to group") + "...");
  connect(action, SIGNAL(activated()), this, SLOT(addToGroup()));
  _actions.insert(AddGroup, action);
}

void ChartPage::createButtonMenu ()
{
  _menu = new QMenu(this);
  _menu->addAction(_actions.value(AddGroup));
  _menu->addSeparator();
  _menu->addAction(_actions.value(ShowAll));
  _menu->addAction(_actions.value(Search));
  _menu->addSeparator();
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

  QSettings settings(g_globalSettings);
  settings.setValue("chart_panel_selected", l2.join(","));
  settings.sync();
  
  Script *script = new Script(this);
  script->setName("ChartPanelAddGroup");
  script->setFile(settings.value("chart_panel_add_group_script").toString());
  script->setCommand("perl");
  script->startScript();
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
  _nav->clearSymbols();

  _nav->setSortingEnabled(FALSE);

  BarData bd;
  bd.setExchange(_searchExchange);
  bd.setSymbol(_searchString);
  
  QuoteDataBase db;
  QStringList l;
  db.search(&bd, l);

  int loop = 0;
  for (; loop < l.count(); loop++)
  {
    QStringList tl = l.at(loop).split(",");
    if (tl.count() != 3)
      continue;
    
    BarData bd;
    bd.setExchange(tl.at(0));
    bd.setSymbol(tl.at(1));
    bd.setName(tl.at(2));
    
    _nav->addSymbol(bd);
  }

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
  _actions.value(AddGroup)->setEnabled(status); 
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
