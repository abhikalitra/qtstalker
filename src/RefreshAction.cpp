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

#include "RefreshAction.h"

#include <QString>
#include <QDebug>
#include <QIcon>
#include <QInputDialog>

#include "Config.h"

#include "../pics/refresh.xpm"

RefreshAction::RefreshAction (QObject *p) : QAction (p)
{
  _timer = 0;
  Config config;
  _minutes = config.getInt(Config::Refresh);

  changeText();
  setIcon(QIcon(refresh_xpm));
  setCheckable(TRUE);
  setChecked(config.getBool(Config::RefreshStatus));
  connect(this, SIGNAL(toggled(bool)), this, SLOT(refreshChart(bool)));

  _timer = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), this, SIGNAL(signalRefresh()));
  _timer->start(60000 * _minutes);
}

void RefreshAction::refreshChart (bool status)
{
  if (status == TRUE)
    _timer->start(60000 * _minutes);
  else
    _timer->stop();

  Config config;
  config.setData((int) Config::RefreshStatus, status);
}

void RefreshAction::refreshUpdated (int minutes)
{
  _minutes = minutes;
  
  if (isChecked())
  {
    _timer->stop();
    _timer->start(60000 * _minutes);
  }

  changeText();

  Config config;
  config.setData(Config::Refresh, _minutes);
}

void RefreshAction::dialog ()
{
  bool ok;
  int t = QInputDialog::getInt(0,
                               tr("Set chart refresh timer"),
                               tr("Enter refresh rate in minutes"),
                               _minutes,
                               1,
                               99,
                               1,
                               &ok,
                               0);

  if (! ok)
    return;

  refreshUpdated(t);
}

void RefreshAction::contextMenuEvent (QContextMenuEvent *)
{
  dialog();
}

void RefreshAction::changeText ()
{
  QString s = tr("Refresh chart every ") + QString::number(_minutes) + tr(" minutes");
  
  setText(s);
  setStatusTip(s);
  setToolTip(s);
}

