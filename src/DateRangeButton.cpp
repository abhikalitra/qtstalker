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
#include "Config.h"
#include "PrefDialog.h"

#include "../pics/date.xpm"
#include "../pics/configure.xpm"

#include <QString>
#include <QDebug>
#include <QCursor>

DateRangeButton::DateRangeButton () 
{
  _menu = new QMenu(this);
  _menu->addAction(QPixmap(configure_xpm), tr("&Configure"), this, SLOT(dialog()), Qt::ALT+Qt::Key_C);
  
  setIcon(QIcon(date));
  setText(tr("Date Range Override"));
  setStatusTip(tr("Date Range Override"));
  setToolTip(tr("Date Range Override"));
  setCheckable(TRUE);

  Config config;
  setChecked(config.getInt(Config::DateRangeButtonStatus));
  
  config.getData(Config::DateRangeStart, _startDate);
  if (! _startDate.isValid())
    _startDate = QDateTime::currentDateTime();
  
  config.getData(Config::DateRangeEnd, _endDate);
  if (! _endDate.isValid())
    _endDate = QDateTime::currentDateTime();

  connect(this, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
}

void DateRangeButton::buttonClicked (bool status)
{
  Config config;
  config.setData(Config::DateRangeButtonStatus, status);

  emit signalDateRangeChanged();
}

void DateRangeButton::dialog ()
{
  int page = 0;
  PrefDialog *dialog = new PrefDialog;
  dialog->setWindowTitle(QObject::tr("Set Date Range"));

  QString s = QObject::tr("Settings");
  dialog->addPage(page, s);

  int pos = 0;
  s = tr("First Date");
  dialog->addDateItem(pos++, page, s, _startDate);

  s = tr("Last Date");
  dialog->addDateItem(pos++, page, s, _endDate);

  int rc = dialog->exec();
  if (rc == QDialog::Rejected)
  {
    delete dialog;
    return;
  }

  pos = 0;
  int flag = 0;
  Config config;
  QDateTime dt;
  dialog->getDate(pos++, dt);
  if (dt != _startDate)
  {
    config.setData(Config::DateRangeStart, dt);
    flag = 1;
    _startDate = dt;
  }
  
  dialog->getDate(pos++, dt);
  if (dt != _endDate)
  {
    config.setData(Config::DateRangeEnd, dt);
    flag = 1;
    _endDate = dt;
  }

  delete dialog;

  if (flag && isChecked())
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

