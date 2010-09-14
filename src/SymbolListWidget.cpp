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

#include "SymbolListWidget.h"

#include <QDebug>

SymbolListWidget::SymbolListWidget ()
{
  setFocusPolicy(Qt::StrongFocus);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  setSortingEnabled(FALSE);

  connect(this, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(itemClicked(QListWidgetItem *)));
}

void SymbolListWidget::clearSymbols ()
{
  clear();
  _symbols.clear();
}

void SymbolListWidget::addSymbol (BarData bd)
{
  _symbols.setSymbol(bd);

  QListWidgetItem *item = new QListWidgetItem;
  item->setText(bd.getKey());
  item->setToolTip(QString(tr("Name: ") + bd.getName()));
  addItem(item);
}

void SymbolListWidget::deleteSymbol ()
{
  QList<QListWidgetItem *> sl = selectedItems();
  if (! sl.count())
    return;

  int loop = 0;
  for (; loop < sl.count(); loop++)
  {
    QListWidgetItem *item = sl.at(loop);
    _symbols.deleteSymbol(item->text());
    delete item;
  }
}

Group & SymbolListWidget::symbols ()
{
  return _symbols;
}

BarData SymbolListWidget::symbol (QString k)
{
  BarData bd;
  _symbols.getSymbol(k, bd);
  return bd;
}

void SymbolListWidget::itemClicked (QListWidgetItem *item)
{
  if (! item)
    return;

  BarData bd;
  if (_symbols.getSymbol(item->text(), bd))
    return;

  emit signalSymbolSelected(bd);
}

void SymbolListWidget::keyPressEvent (QKeyEvent *e)
{
  switch (e->key())
  {
    case Qt::Key_Enter:
    case Qt::Key_Return:
      emit itemClicked(currentItem());
      break;
    default:
      QListWidget::keyPressEvent(e);
      break;
  }
}

