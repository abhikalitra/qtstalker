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
#include "PrefDialog.h"

#include <QInputDialog>
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
    setTabPosition(East);

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
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(tr("Chart Tab Settings"));
  QString s = tr("Settings");
  int page = 0;
  dialog->addPage(page, s);

  Config config;
  int pos = 0;

  QStringList l;
  l << tr("North") << tr("South") << tr("West") << tr("East");
  config.getData(_id, s);
  int tp = (int) tabPosition();
  dialog->addComboItem(pos++, page, QString("Position"), l, tp);

  // north south height
  s = tr("North / South Height");
  QString key = _id + "NSH";
  QString d;
  config.getData(key, d);
  int nsh = d.toInt();
  dialog->addIntItem(pos++, page, s, nsh, 1, 100);

  // north south width
  s = tr("North / South Width");
  key = _id + "NSW";
  config.getData(key, d);
  int nsw = d.toInt();
  dialog->addIntItem(pos++, page, s, nsw, 1, 100);

  // east west height
  s = tr("East / West Height");
  key = _id + "EWH";
  config.getData(key, d);
  int ewh = d.toInt();
  dialog->addIntItem(pos++, page, s, ewh, 1, 100);

  // east west width
  s = tr("East / West Width");
  key = _id + "EWW";
  config.getData(key, d);
  int eww = d.toInt();
  dialog->addIntItem(pos++, page, s, eww, 1, 100);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  int flag = 0;
  pos = 0;
  dialog->getCombo(pos++, s);
  int index = l.indexOf(s);
  if (index != tp)
  {
    d = QString::number(index);
    config.setData(_id, d);
    setTabPosition((QTabWidget::TabPosition) index);
    flag = 1;
  }

  int t = dialog->getInt(pos++);
  if (t != nsh)
  {
    key = _id + "NSH";
    d = QString::number(t);
    config.setData(key, d);
    flag = 1;
  }

  t = dialog->getInt(pos++);
  if (t != nsw)
  {
    key = _id + "NSW";
    d = QString::number(t);
    config.setData(key, d);
    flag = 1;
  }

  t = dialog->getInt(pos++);
  if (t != ewh)
  {
    key = _id + "EWH";
    d = QString::number(t);
    config.setData(key, d);
    flag = 1;
  }
  
  t = dialog->getInt(pos++);
  if (t != eww)
  {
    key = _id + "EWW";
    d = QString::number(t);
    config.setData(key, d);
    flag = 1;
  }

  delete dialog;

  if (flag)
    setTabSizes();
}














