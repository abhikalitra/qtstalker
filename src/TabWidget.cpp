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
  config.transaction();

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

  config.commit();

  setTabSizes();

  connect(this, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged()));
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

  p->setStartIndex(p->index());
}

void TabWidget::setTabButton (int pos, QString text)
{
  QString s = text;

  // if label is too big for tab, then shorten label to fit
  QFont f;
  QFontMetrics fm(f);
  int width = fm.width(s);

  Config config;
  QString key = _id + "EWW";
  QString d;
  config.getData(key, d);
  int tabWidth = d.toInt();
  if (width > tabWidth)
  {
    QString s2;
    while (width > tabWidth)
    {
      s2 = s;
      s2.remove(s2.length() / 2, 1);
      s = s2;
      s2.insert(s2.length() / 2, "*");
      width = fm.width(s2);
    }

    s = s2;
  }
  
  // create a label that paints horizontally
  QTabBar *tb = tabBar();
  QLabel *label = new QLabel;
  label->setIndent(0);
  label->setMargin(0);
  label->setAlignment(Qt::AlignLeft);
  label->setText(s);
  tb->setTabButton(pos, QTabBar::LeftSide, label);
}



