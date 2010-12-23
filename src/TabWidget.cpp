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
#include "Globals.h"

#include <QCursor>
#include <QLabel>
#include <QSettings>

TabWidget::TabWidget (QString &id)
{
  _id = "tab_widget_" + id;
  
  _menu = new QMenu(this);
  _menu->addAction(tr("Tab &Settings"), this, SLOT(tabDialog()), Qt::ALT+Qt::Key_S);

  setDocumentMode(TRUE); // remove tab frame, get a few more pixels of space
  setContentsMargins(0, 0, 0, 0);
  
  QSettings settings(g_settingsFile);
  setTabPosition((QTabWidget::TabPosition) settings.value(_id, East).toInt());

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
  QSettings settings(g_settingsFile);

  QString key, w, h;
  if (tabPosition() == North || tabPosition() == South)
  {
    key = _id + "_NSW";
    w = settings.value(key, "60").toString();

    key = _id + "_NSH";
    h = settings.value(key, "14").toString();
  }
  else
  {
    key = _id + "_EWW";
    w = settings.value(key, "60").toString();

    key = _id + "_EWH";
    h = settings.value(key, "6").toString();
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
  QSettings settings(g_settingsFile);
  setTabPosition((QTabWidget::TabPosition) settings.value(_id).toInt());

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

  QSettings settings(g_settingsFile);
  
  QString key = _id + "_EWW";
  QString tabWidth = settings.value(key, "60").toString();
  if (width > tabWidth.toInt())
  {
    QString s2;
    while (width > tabWidth.toInt())
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
