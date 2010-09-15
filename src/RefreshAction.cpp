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
#include "Config.h"
#include "../pics/refresh.xpm"

#include <QString>
#include <QDebug>
#include <QIcon>
#include <QInputDialog>

RefreshAction::RefreshAction ()
{
  _timer = 0;
  
  Config config;
  _minutes = config.getInt(Config::Refresh);
  if (! _minutes)
  {
    _minutes = 1;
    config.setData(Config::Refresh, _minutes);
  }

  changeText();
  setIcon(QIcon(refresh_xpm));
  setCheckable(TRUE);

  QString s;
  config.getData(Config::RefreshStatus, s);
  if (s.isEmpty())
  {
    setChecked(0);
    config.setData(Config::RefreshStatus, 0);
  }
  else
    setChecked(s.toInt());
  
  connect(this, SIGNAL(toggled(bool)), this, SLOT(refreshChart(bool)));

  _timer = new QTimer(this);
  connect(_timer, SIGNAL(timeout()), this, SIGNAL(signalRefresh()));

  if (isChecked())
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
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle(tr("Qtstalker: Set Chart Refresh Timer"));
  dialog->setLabelText(tr("Enter refresh rate in minutes"));
  dialog->setInputMode(QInputDialog::IntInput);
  dialog->setIntRange(1, 99);
  dialog->setIntStep(1);
  connect(dialog, SIGNAL(intValueSelected(int)), this, SLOT(refreshUpdated(int)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void RefreshAction::changeText ()
{
  QString s = tr("Refresh chart every ") + QString::number(_minutes) + tr(" minutes");
  
  setText(s);
  setStatusTip(s + tr(" Right click mouse for options."));
  setToolTip(s);
}

void RefreshAction::contextMenuEvent (QContextMenuEvent *)
{
  dialog();
}

