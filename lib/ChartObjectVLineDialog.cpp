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

#include "ChartObjectVLineDialog.h"
#include "Config.h"
#include "ChartObject.h"
#include "ChartObjectDataBase.h"
#include "Globals.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QString>

ChartObjectVLineDialog::ChartObjectVLineDialog ()
{
  setWindowTitle(tr("Edit VLine Chart Object"));

  createMainPage();
}

void ChartObjectVLineDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // date
  QLabel *label = new QLabel(tr("Date"));
  grid->addWidget(label, row, col++);

  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  grid->addWidget(_date, row++, col--);


  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  grid->addWidget(_color, row++, col--);
  _color->setColorButton();

  
  // default
  label = new QLabel(tr("Set as default"));
  grid->addWidget(label, row, col++);

  _default = new QCheckBox;
  grid->addWidget(_default, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectVLineDialog::done ()
{
  if (_default->isChecked())
  {
    Config config;
    config.setData((int) Config::DefaultChartObjectVLineColor, _color);
  }

  _settings.color = _color->color();
  _settings.date = _date->dateTime();

  ChartObjectDataBase db;
  g_mutex.lock();
  db.setChartObject(_settings);
  g_mutex.unlock();

  emit signalDone(_settings);

  accept();
}

void ChartObjectVLineDialog::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  
  _color->setColor(_settings.color);
  _date->setDateTime(_settings.date);
}

