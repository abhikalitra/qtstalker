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
#include <qlayout.h>
#include <qcursor.h>
#include <qdir.h>

IndicatorPage::IndicatorPage (QWidget *w) : QWidget (w)
{
  QVBoxLayout *vbox = new QVBoxLayout(this);
  vbox->setMargin(2);
  vbox->setSpacing(5);
  
  list = new QListBox(this);
  connect(list, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  connect(list, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(list, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));
  vbox->addWidget(list);

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
    menu->setItemEnabled(menu->idAt(3), TRUE);
  }
  else
  {
    menu->setItemEnabled(menu->idAt(1), FALSE);
    menu->setItemEnabled(menu->idAt(2), FALSE);
    menu->setItemEnabled(menu->idAt(3), FALSE);
  }
}

void IndicatorPage::newIndicator ()
{
  emit signalNewIndicator();
}

void IndicatorPage::editIndicator ()
{
  emit signalEditIndicator(list->currentText());
}

void IndicatorPage::deleteIndicator ()
{
  emit signalDeleteIndicator(list->currentText());
}

void IndicatorPage::refreshList ()
{
  list->clear();
  statusList.clear();

  Config config;
  QDir dir(config.getData(Config::IndicatorPath));
  QStringList l = QStringList::split(",", config.getData(Config::IndicatorPageStatus), FALSE);
  int loop;
  for (loop = 2; loop < (int) dir.count(); loop++)
  {
    if (l.findIndex(dir[loop]) == -1)
    {
      list->insertItem(ok, dir[loop], -1);
      statusList.setData(dir[loop], "1");
    }
    else
    {
      list->insertItem(disable, dir[loop], -1);
      statusList.setData(dir[loop], "0");
    }
  }
}

void IndicatorPage::doubleClick (QListBoxItem *item)
{
  if (! item)
    return;
    
  QString s = item->text();
  int index = list->index(item);
  if (statusList.getInt(s))
  {
    list->changeItem(disable, s, index);
    statusList.setData(s, "0");
    emit signalDisableIndicator(s);
  }
  else
  {
    list->changeItem(ok, s, index);
    statusList.setData(s, "1");
    emit signalEnableIndicator(s);
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

