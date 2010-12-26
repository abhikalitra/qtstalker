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

#include "DockWidget.h"

#include <QDebug>

DockWidget::DockWidget (QString title, QWidget *p) : QDockWidget (title, p)
{
  _menu = new QMenu(this);
  _menu->addAction(tr("&Lock"), this, SLOT(lock()), Qt::ALT+Qt::Key_L);
  _menu->addAction(tr("&Unlock"), this, SLOT(unlock()), Qt::ALT+Qt::Key_U);

  _titleBar = titleBarWidget();
  _noTitleBar = new QWidget(this);
}

void DockWidget::lock ()
{
  setTitleBarWidget(_noTitleBar);
  emit signalLockStatus(TRUE);
}

void DockWidget::unlock ()
{
  setTitleBarWidget(_titleBar);
  emit signalLockStatus(FALSE);
}

void DockWidget::statusChanged (bool status)
{
  if (status)
    lock();
  else
    unlock();
}

void DockWidget::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}
