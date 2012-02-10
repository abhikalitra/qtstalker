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

#include "DialogEAVDataBaseSearch.h"
#include "WindowTitle.h"
#include "ScriptFunctions.h"
#include "TypeOperator.h"
#include "EAVDataBase.h"
#include "EAVSearch.h"

#include "../pics/search.xpm"

#include <QtDebug>

DialogEAVDataBaseSearch::DialogEAVDataBaseSearch (QWidget *p) : Dialog (p)
{
  _keySize = "dialog_eav_data_base_search_window_size";
  _keyPos = "dialog_eav_data_base_search_window_position";

  WindowTitle wt;
  setWindowTitle(wt.title(tr("Database Search"), QString()));

  createGUI();

  loadSettings();

//  okButtonStatus();
}

void DialogEAVDataBaseSearch::createGUI ()
{
  int pos = 1;
  TypeOperator top;
  ScriptFunctions sf;
  QStringList tl;
  sf.keys(tl);
  
  // first search line
  _gbox1 = new QGroupBox(tr("Search 1"));
  _gbox1->setCheckable(TRUE);
  _vbox->insertWidget(pos++, _gbox1);
  
  QHBoxLayout *hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  _gbox1->setLayout(hbox);
  
  _keys1 = new QComboBox;
  _keys1->addItems(tl);
  hbox->addWidget(_keys1);
  
  _op1 = new QComboBox;
  _op1->addItems(top.list());
  hbox->addWidget(_op1);
  
  _value1 = new WidgetLineEdit(this);
  hbox->addWidget(_value1, 1);
  
  // second search line
  _gbox2 = new QGroupBox(tr("Search 2"));
  _gbox2->setCheckable(TRUE);
  _vbox->insertWidget(pos++, _gbox2);
  
  hbox = new QHBoxLayout;
  hbox->setSpacing(2);
  hbox->setMargin(0);
  _gbox2->setLayout(hbox);
  
  _keys2 = new QComboBox;
  _keys2->addItems(tl);
  hbox->addWidget(_keys2);
  
  _op2 = new QComboBox;
  _op2->addItems(top.list());
  hbox->addWidget(_op2);
  
  _value2 = new WidgetLineEdit(this);
  hbox->addWidget(_value2, 1);
  
  // list area
  _nameList = new QListWidget;
  _nameList->setSortingEnabled(FALSE);
  _nameList->setSelectionMode(QAbstractItemView::NoSelection);
  _vbox->insertWidget(pos++, _nameList);
  
  // add search button
  _searchButton = new QPushButton(tr("Search"));
  _searchButton->setIcon(QIcon(search_xpm));
  connect(_searchButton, SIGNAL(clicked()), this, SLOT(search()));
  _buttonBox->addButton(_searchButton, QDialogButtonBox::ActionRole);

  // save some unused space
//  _message->hide();
}

/*
void DialogEAVDataBaseSearch::searchSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _searchList->selectedItems();
  if (sl.count())
    status = 1;

  _addButton->setEnabled(status);
}
*/
/*
void DialogEAVDataBaseSearch::symbolSelectionChanged ()
{
  int status = 0;
  QList<QListWidgetItem *> sl = _symbolList->selectedItems();
  if (sl.count())
    status = 1;

  _deleteButton->setEnabled(status);
}
*/

void DialogEAVDataBaseSearch::done ()
{
  saveSettings();

  QStringList l;
  int loop = 0;
  for (; loop < _nameList->count(); loop++)
    l << _nameList->item(loop)->text();

  emit signalDone(l);

  accept();
}

/*
void DialogEAVDataBaseSearch::addButtonPressed ()
{
  QList<QListWidgetItem *> sl = _searchList->selectedItems();

  KeySymbol keys;
  int loop = 0;
  for (; loop < sl.size(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    
    Symbol symbol;
    symbol.set(keys.indexToString(KeySymbol::_SYMBOL), Data(item->text()));
    symbol.set(keys.indexToString(KeySymbol::_NAME), Data(item->toolTip()));
    
    _symbolList->addSymbol(symbol);
  }
  
  okButtonStatus();
}
*/
/*
void DialogEAVDataBaseSearch::deleteButtonPressed ()
{
  QList<QListWidgetItem *> sl = _symbolList->selectedItems();

  int loop = 0;
  for (; loop < sl.size(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    delete item;
  }

  symbolSelectionChanged();
  okButtonStatus();
}
*/

void DialogEAVDataBaseSearch::search ()
{
  _nameList->clear();
  
  EAVSearch search;
  
  if (_gbox1->isChecked())
    search.append(_keys1->currentText(), _op1->currentText(), _value1->text());
  
  if (_gbox2->isChecked())
    search.append(_keys2->currentText(), _op2->currentText(), _value2->text());
  
  QStringList names;
  EAVDataBase db("scripts");
  if (db.search(search, names))
  {
    return;
  }
  
  _nameList->addItems(names);
}
