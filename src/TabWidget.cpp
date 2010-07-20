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

#include "TabWidget.h"

#include <QInputDialog>
#include <QCursor>
#include <QLabel>

TabWidget::TabWidget (QString &id)
{
  _id = "TabWidget" + id;
  
  _menu = new QMenu(this);
  _menu->addAction(tr("Tab &Position"), this, SLOT(tabPositionDialog()), Qt::ALT+Qt::Key_P);

  setDocumentMode(TRUE); // remove tab frame, get a few more pixels of space
  setContentsMargins(0, 0, 0, 0);
  
  Config config;
  QString d;
  config.getData(_id, d);
  if (! d.isEmpty())
    setTabPosition((QTabWidget::TabPosition) d.toInt());
  else
    setTabPosition(East);

  setTabHeight();
}

void TabWidget::tabPositionDialog ()
{
  QStringList l;
  l << tr("North") << tr("South") << tr("West") << tr("East");

  bool ok;
  QString item = QInputDialog::getItem(this,
                                       tr("Chart Tab Position"),
                                       tr("Select position"),
                                       l,
                                       (int) tabPosition(),
                                       FALSE,
                                       &ok,
                                       0);
  if (! ok)
    return;

  Config config;
  int index = l.indexOf(item);
  QString d = QString::number(index);
  config.setData(_id, d);

  setTabPosition((QTabWidget::TabPosition) index);
  setTabHeight();
}

void TabWidget::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}

QTabBar * TabWidget::getTabBar ()
{
  return tabBar();
}

void TabWidget::setTabHeight ()
{
  // make the tab narrower
  if (tabPosition() == North || tabPosition() == South)
    setStyleSheet("QTabBar::tab { height: 14px;}");
  else
    setStyleSheet("QTabBar::tab { height: 6px; width: 60px; }");
}

