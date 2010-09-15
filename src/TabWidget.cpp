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
#include "TabWidgetDialog.h"
#include "Plot.h"

#include <QCursor>
#include <QLabel>

TabWidget::TabWidget (QString &id)
{
  _id = "TabWidget" + id;
  
  _menu = new QMenu(this);
  _menu->addAction(tr("Tab &Settings"), this, SLOT(tabDialog()), Qt::ALT+Qt::Key_S);

  setDocumentMode(TRUE); // remove tab frame, get a few more pixels of space
  setContentsMargins(0, 0, 0, 0);
  
  Config config;
  QString d;
  config.getData(_id, d);
  if (! d.isEmpty())
    setTabPosition((QTabWidget::TabPosition) d.toInt());
  else
  {
    setTabPosition(East);
    d = QString::number(tabPosition());
    config.setData(_id, d);
  }

  QString key = _id + "NSW";
  config.getData(key, d);
  if (d.isEmpty())
  {
    d = "60";
    config.setData(key, d);
  }

  key = _id + "NSH";
  config.getData(key, d);
  if (d.isEmpty())
  {
    d = "14";
    config.setData(key, d);
  }

  key = _id + "EWW";
  config.getData(key, d);
  if (d.isEmpty())
  {
    d = "60";
    config.setData(key, d);
  }

  key = _id + "EWH";
  config.getData(key, d);
  if (d.isEmpty())
  {
    d = "6";
    config.setData(key, d);
  }

  setTabSizes();

//  connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged()));
}

void TabWidget::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}

QTabBar * TabWidget::getTabBar ()
{
  return tabBar();
}

void TabWidget::setTabSizes ()
{
  Config config;
  QString key, w, h;
  if (tabPosition() == North || tabPosition() == South)
  {
    key = _id + "NSW";
    config.getData(key, w);

    key = _id + "NSH";
    config.getData(key, h);
  }
  else
  {
    key = _id + "EWW";
    config.getData(key, w);

    key = _id + "EWH";
    config.getData(key, h);
  }
    
  setStyleSheet("QTabBar::tab { width:" + w + "px; height:" + h + "px;}");

  // make the tab narrower
//  if (tabPosition() == North || tabPosition() == South)
//    setStyleSheet("QTabBar::tab { height: 14px;}");
//  else
//    setStyleSheet("QTabBar::tab { height: 6px; width: 60px; }");
}

void TabWidget::tabDialog ()
{
  TabWidgetDialog *dialog = new TabWidgetDialog(_id);
  connect(dialog, SIGNAL(signalChanged()), this, SLOT(tabDialog2()));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void TabWidget::tabDialog2 ()
{
  QString d;
  Config config;
  config.getData(_id, d);
  if (! d.isEmpty())
    setTabPosition((QTabWidget::TabPosition) d.toInt());

  setTabSizes();
}

void TabWidget::currentTabChanged ()
{
  Plot *p = (Plot *) currentWidget();
  if (! p)
    return;

  p->replot();
}
