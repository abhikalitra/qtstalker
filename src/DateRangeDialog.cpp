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

#include "DateRangeDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QFormLayout>

DateRangeDialog::DateRangeDialog (QDateTime &sd, QDateTime &ed)
{
  _sd = sd;
  _ed = ed;
  _helpFile = "main.html";
  
  setWindowTitle("Qtstalker" + g_session + ": " +  tr("Set Date Range"));

  createMainPage();
}

void DateRangeDialog::createMainPage ()
{
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // first date
  _start = new QDateTimeEdit(_sd);
  _start->setCalendarPopup(TRUE);
  form->addRow(tr("First Date"), _start);
  
  // last date
  _end = new QDateTimeEdit(_ed);
  _end->setCalendarPopup(TRUE);
  form->addRow(tr("Last Date"), _end);

  _tabs->addTab(w, tr("Date Range"));
}

void DateRangeDialog::done ()
{
  int flag = 0;
  QDateTime sd = _start->dateTime();
  if (sd != _sd)
    flag = 1;

  QDateTime ed = _end->dateTime();
  if (ed != _ed)
    flag = 1;

  if (flag)
    emit signalDateChanged(sd, ed);

  accept();
}

