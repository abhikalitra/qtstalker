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
#include "Macro.h"
#include "help.xpm"
#include "ok.xpm"
#include "disable.xpm"
#include "edit.xpm"
#include "delete.xpm"
#include "newchart.xpm"
#include <qcursor.h>
#include <qdir.h>
#include <qaccel.h>

IndicatorPage::IndicatorPage (QWidget *w) : QListBox (w)
{
  keyFlag = FALSE;
  
  connect(this, SIGNAL(doubleClicked(QListBoxItem *)), this, SLOT(doubleClick(QListBoxItem *)));
  connect(this, SIGNAL(contextMenuRequested(QListBoxItem *, const QPoint &)), this,
          SLOT(rightClick(QListBoxItem *)));
  connect(this, SIGNAL(highlighted(const QString &)), this, SLOT(itemSelected(const QString &)));

  menu = new QPopupMenu(this);
  menu->insertItem(QPixmap(newchart), tr("&New Indicator	Ctrl+N"), this, SLOT(newIndicator()));
  menu->insertItem(QPixmap(edit), tr("&Edit Indicator		Ctrl+E"), this, SLOT(editIndicator()));
  menu->insertItem(QPixmap(deleteitem), tr("&Delete Indicator	Ctrl+D"), this, SLOT(deleteIndicator()));
  menu->insertSeparator(-1);
  menu->insertItem(QPixmap(help), tr("&Help		Ctrl+H"), this, SLOT(slotHelp()));

  QAccel *a = new QAccel(this);
  connect(a, SIGNAL(activated(int)), this, SLOT(slotAccel(int)));
  a->insertItem(CTRL+Key_N, NewIndicator);
  a->insertItem(CTRL+Key_D, DeleteIndicator);
  a->insertItem(CTRL+Key_E, EditIndicator);
  a->insertItem(CTRL+Key_H, Help);
}

IndicatorPage::~IndicatorPage ()
{
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

bool IndicatorPage::getIndicatorStatus (QString d)
{
  return (bool) statusList.getInt(d);
}

void IndicatorPage::setKeyFlag (bool d)
{
  keyFlag = d;
}

void IndicatorPage::keyPressEvent (QKeyEvent *key)
{
  if (keyFlag)
    emit signalKeyPressed (Macro::IndicatorPage, key->state(), key->key(), key->ascii(), key->text());
    
  doKeyPress(key);  
}

void IndicatorPage::doKeyPress (QKeyEvent *key)
{
  key->accept();
  
  if (key->state() == Qt::ControlButton)
  {
    switch(key->key())
    {
      case Qt::Key_N:
        slotAccel(NewIndicator);
	break;
      case Qt::Key_D:
        slotAccel(DeleteIndicator);
	break;
      case Qt::Key_E:
        slotAccel(EditIndicator);
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
        break;      
      case Qt::Key_Enter:
      case Qt::Key_Return:
        changeIndicator(currentText(), currentItem());
        break;
      default:
        QListBox::keyPressEvent(key);
        break;
    }
  }
}

void IndicatorPage::slotAccel (int id)
{
  switch (id)
  {
    case NewIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_N, 0, QString());
      newIndicator();
      break;  
    case DeleteIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_D, 0, QString());
      deleteIndicator();
      break;  
    case EditIndicator:
      if (keyFlag)
        emit signalKeyPressed (Macro::IndicatorPage, ControlButton, Key_E, 0, QString());
      editIndicator();
      break;  
    case Help:
      slotHelp();
      break;  
    default:
      break;
  }
}

