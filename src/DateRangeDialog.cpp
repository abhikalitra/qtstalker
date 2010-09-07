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

#include <QtDebug>
#include <QMessageBox>
#include <QLayout>
#include <QLabel>
#include <QStringList>

DateRangeDialog::DateRangeDialog (QDateTime &sd, QDateTime &ed) : QDialog (0, 0)
{
  _sd = sd;
  _ed = ed;
  
  setAttribute(Qt::WA_DeleteOnClose);
  
  setWindowTitle(tr("Qtstalker: Set Date Range"));

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(10);
  vbox->setMargin(5);
  setLayout(vbox);

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  vbox->addLayout(grid);

  int row = 0;
  int col = 0;

  // first date
  QLabel *label = new QLabel(tr("First Date"));
  grid->addWidget(label, row, col++);

  _start = new QDateTimeEdit(sd);
  _start->setCalendarPopup(TRUE);
  grid->addWidget(_start, row++, col--);
  
  // last date
  label = new QLabel(tr("Last Date"));
  grid->addWidget(label, row, col++);

  _end = new QDateTimeEdit(ed);
  _end->setCalendarPopup(TRUE);
  grid->addWidget(_end, row++, col--);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel | QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
  vbox->addWidget(_buttonBox);

  vbox->addStretch(1);
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

