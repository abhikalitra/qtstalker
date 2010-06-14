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

TabWidget::TabWidget (int row)
{
  switch (row)
  {
    case 0:
      _rowPositionParm = Config::IndicatorRow1Position;
      _lastIndexParm = Config::LastIndicatorRow1;
      break;
    case 1:
      _rowPositionParm = Config::IndicatorRow2Position;
      _lastIndexParm = Config::LastIndicatorRow2;
      break;
    case 2:
      _rowPositionParm = Config::IndicatorRow3Position;
      _lastIndexParm = Config::LastIndicatorRow3;
      break;
    default:
      break;
  }

  _menu = new QMenu(this);
  _menu->addAction(tr("Tab Rows"), this, SLOT(tabRowsDialog()), Qt::ALT+Qt::Key_R);
  _menu->addAction(tr("Tab &Position"), this, SLOT(tabPositionDialog()), Qt::ALT+Qt::Key_P);

  setDocumentMode(TRUE); // remove tab frame, get a few more pixels of space
  setContentsMargins(0, 0, 0, 0);
  
  Config config;
  setTabPosition((QTabWidget::TabPosition) config.getInt(_rowPositionParm));
}

void TabWidget::tabRowsDialog ()
{
  Config config;
  int t = config.getInt(Config::IndicatorTabRows);
  
  bool ok;
  t = QInputDialog::getInt(0,
                           tr("Chart Tab Rows"),
                           tr("Enter number of chart tab rows"),
                           t,
                           1,
                           3,
                           1,
                           &ok,
                           0);

  if (! ok)
    return;

  config.setData(Config::IndicatorTabRows, t);
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
                                       0,
                                       FALSE,
                                       &ok,
                                       0);
  if (! ok)
    return;

  Config config;
  
  switch (l.indexOf(item))
  {
    case 0:
      config.setData(_rowPositionParm, North);
      setTabPosition(North);
      break;
    case 1:
      config.setData(_rowPositionParm, South);
      setTabPosition(South);
      break;
    case 2:
      config.setData(_rowPositionParm, West);
      setTabPosition(West);
      break;
    case 3:
      config.setData(_rowPositionParm, East);
      setTabPosition(East);
      break;
    default:
      break;
  }
}

void TabWidget::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}

void TabWidget::saveCurrentTab ()
{
  // save last indicators used
  Config config;
  config.setData(_lastIndexParm, currentIndex());
}

void TabWidget::loadCurrentTab ()
{
  // set last indicators used
  Config config;
  setCurrentIndex(config.getInt(_lastIndexParm));
}

void TabWidget::deleteTab (QString text)
{
  int loop = 0;
  for (; loop < count(); loop++)
  {
    if (QString::compare(tabText(loop), text) == 0)
    {
      removeTab(loop);
      break;
    }
  }
}


