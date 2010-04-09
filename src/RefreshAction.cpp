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

#include "Config.h"

#include "../pics/refresh.xpm"

RefreshAction::RefreshAction (QToolBar *tb) : QAction (tb)
{
  timer = 0;
  Config config;
  
  setIcon(QIcon(refresh_xpm));
  setStatusTip(tr("Refresh chart every x minutes"));
  setToolTip(tr("Refresh chart every x minutes"));
  setCheckable(TRUE);
  setChecked(config.getBool(Config::RefreshStatus));
  connect(this, SIGNAL(toggled(bool)), this, SLOT(refreshChart(bool)));
  tb->addAction(this);
}

void RefreshAction::refreshChart (bool status)
{
  if (status == TRUE)
  {
    Config config;
    int minutes = config.getInt(Config::Refresh);

    if (timer)
      delete timer;

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SIGNAL(signalRefresh()));
    timer->start(60000 * minutes);
  }
  else
  {
    if (timer)
      delete timer;
    timer = 0;
  }

  Config config;
  config.setData((int) Config::RefreshStatus, status);
}

void RefreshAction::refreshUpdated (int minutes)
{
  if (! isChecked())
    return;
  
  if (timer)
    delete timer;

  timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SIGNAL(signalRefresh()));
  timer->start(60000 * minutes);
}

