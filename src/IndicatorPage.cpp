/*
 *  Qtstalker stock charter
 *
 *  Copyright (C) 2001-2004 Stefan S. Stratigakos
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

#include "IndicatorPage.h"
#include "HelpWindow.h"
#include "Config.h"
#include "help.xpm"
#include "ok.xpm"
#include "disable.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "newchart.xpm"
#include <qcursor.h>
#include <qdir.h>

IndicatorPage::IndicatorPage (QWidget *w) : QListBox (w)
{
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));

  menu = new QPopupMenu();
  menu->insertItem(QPixmap(newchart), tr("&New Indicator"), this, SLOT(newIndicator()), CTRL+Key_N);
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator"), this, SLOT(editIndicator()), CTRL+Key_E);
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator"), this, SLOT(deleteIndicator()), CTRL+Key_D);
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help"), this, SLOT(slotHelp()), CTRL+Key_H);
}

IndicatorPage::~IndicatorPage ()
{
  delete menu;
}

void IndicatorPage::itemSelected (const QString &d)
{
  if (d.length())
  {
    menu->setItemEnabled(menu->idAt(1), TRUE);
    menu->setItemEnabled(menu->idAt(2), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
  }
}

void IndicatorPage::newIndicator ()
{
  emit signalNewIndicator();
}

void IndicatorPage::editIndicator ()
{
  emit signalEditIndicator(currentText());
}

void IndicatorPage::deleteIndicator ()
{
  emit signalDeleteIndicator(currentText());
}

void IndicatorPage::refreshList ()
{
  clear();
  statusList.clear();

  Config config;
  QDir dir(config.getData(Config::IndicatorPath));
  QStringList l = QStringList::split(",", config.getData(Config::IndicatorPageStatus), FALSE);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    if (l.findIndex(dir[loop]) == -1)
    {
      insertItem(ok, dir[loop], -1);
      statusList.setData(dir[loop], "1");
    }
    else
    {
      insertItem(disable, dir[loop], -1);
      statusList.setData(dir[loop], "0");
    }
  }
}

void IndicatorPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  changeIndicator(item->text(), index(item));
}

void IndicatorPage::changeIndicator (QString d, int i)
{
  if (! d.length())
    return;
    
  if (statusList.getInt(d))
  {
    changeItem(disable, d, i);
    statusList.setData(d, "0");
    emit signalDisableIndicator(d);
  }
  else
  {
    changeItem(ok, d, i);
    statusList.setData(d, "1");
    emit signalEnableIndicator(d);
  }
}

void IndicatorPage::slotHelp ()
{
  HelpWindow *hw = new HelpWindow(this, "workwithmainindicators.html");
  hw->show();
}

void IndicatorPage::rightClick (QListBoxItem *)
{
  menu->exec(QCursor::pos());
}

void IndicatorPage::saveStatus ()
{
  QStringList l = statusList.getKeyList();
  QStringList l2;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (! statusList.getInt(l[loop]))
      l2.append(l[loop]);
  }

  Config config;
  config.setData(Config::IndicatorPageStatus, l2.join(","));
}

QStringList IndicatorPage::getDisabledIndicators ()
{
  QStringList l = statusList.getKeyList();
  QStringList l2;
  int loop;
  for (loop = 0; loop < (int) l.count(); loop++)
  {
    if (! statusList.getInt(l[loop]))
      l2.append(l[loop]);
  }

  return l2;
}

void IndicatorPage::keyPressEvent (QKeyEvent *key)
{
  emit signalKeyPressed (2, key);
  doKeyPress(key);  
}

void IndicatorPage::doKeyPress (QKeyEvent *key)
{
  if (key->state() == Qt::ControlButton)
  {
    switch (key->key())
    {
      case Qt::Key_N:
        key->accept();
        newIndicator();
        break;
      case Qt::Key_E:
        key->accept();
        editIndicator();
        break;
      case Qt::Key_Delete:
        key->accept();
        deleteIndicator();
        break;
      case Qt::Key_H:
        key->accept();
        slotHelp();
        break;
      default:
        break;
    }
  }
  else
  {
    switch (key->key())
    {
      case Qt::Key_Left: // segfaults if we dont trap this
      case Qt::Key_Right: // segfaults if we dont trap this
        key->accept();
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        key->accept();
        changeIndicator(currentText(), currentItem());
        break;
      default:
        key->ignore();
        QListBox::keyPressEvent(key);
        break;
    }
  }
}
