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

#include "DateRangeButton.h"
#include "DateRangeDialog.h"
#include "Globals.h"

#include "../pics/date.xpm"
#include "../pics/configure.xpm"

#include <QString>
#include <QDebug>
#include <QCursor>
#include <QSettings>

DateRangeButton::DateRangeButton () 
{
  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(dialog()), Qt::ALT+Qt::Key_C);
  
  setIcon(QIcon(date));
  setText(tr("Date Range Override"));
  setStatusTip(tr("Date Range Override"));
  setToolTip(tr("Date Range Override"));
  setCheckable(TRUE);

  QSettings settings(g_settingsFile);
  setChecked(settings.value("date_range_button_status", 0).toInt());

  _startDate = settings.value("date_range_start", QDateTime::currentDateTime()).toDateTime();
  
  _endDate = settings.value("date_range_end", QDateTime::currentDateTime()).toDateTime();

  connect(this, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
}

void DateRangeButton::buttonClicked (bool status)
{
  QSettings settings(g_settingsFile);
  settings.setValue("date_range_button_status", status);
  settings.sync();

  emit signalDateRangeChanged();
}

void DateRangeButton::dialog ()
{
  DateRangeDialog *dialog = new DateRangeDialog(_startDate, _endDate);
  connect(dialog, SIGNAL(signalDateChanged(QDateTime, QDateTime)), this, SLOT(dateChanged(QDateTime, QDateTime)));
  connect(dialog, SIGNAL(finished(int)), dialog, SLOT(deleteLater()));
  dialog->show();
}

void DateRangeButton::dateChanged (QDateTime sd, QDateTime ed)
{
  QSettings settings(g_settingsFile);
  settings.setValue("date_range_start", sd);
  settings.setValue("date_range_end", ed);
  settings.sync();
  
  _startDate = sd;
  _endDate = ed;

  if (isChecked())
    emit signalDateRangeChanged();
}

void DateRangeButton::contextMenuEvent (QContextMenuEvent *)
{
  _menu->exec(QCursor::pos());
}

QDateTime & DateRangeButton::startDate ()
{
  return _startDate;
}

QDateTime & DateRangeButton::endDate ()
{
  return _endDate;
}
