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
#include "../pics/refresh.xpm"
#include "Globals.h"

#include <QString>
#include <QDebug>
#include <QIcon>
#include <QInputDialog>
#include <QSettings>

RefreshAction::RefreshAction ()
{
  _timer = 0;
  
  QSettings settings;
  settings.beginGroup("main" + g_session);

  _minutes = settings.value("refresh", 1).toInt();

  changeText();
  setIcon(QIcon(refresh_xpm));
  setCheckable(TRUE);

  setChecked(settings.value("refresh_status", 0).toInt());

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

  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("refresh_status", status);
  settings.sync();
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

  QSettings settings;
  settings.beginGroup("main" + g_session);
  settings.setValue("refresh", _minutes);
  settings.sync();
}

void RefreshAction::dialog ()
{
  QInputDialog *dialog = new QInputDialog;
  dialog->setWindowTitle("Qtstalker" + g_session + ": " + tr("Set Chart Refresh Timer"));
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

