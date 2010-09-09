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

#include "ChartObjectTLineDialog.h"
#include "Config.h"
#include "ChartObject.h"
#include "Globals.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QString>

ChartObjectTLineDialog::ChartObjectTLineDialog ()
{
  setWindowTitle(tr("Edit TLine Chart Object"));

  createMainPage();
}

void ChartObjectTLineDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // date
  QLabel *label = new QLabel(tr("Start Date"));
  grid->addWidget(label, row, col++);

  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  grid->addWidget(_date, row++, col--);


  // date2
  label = new QLabel(tr("End Date"));
  grid->addWidget(label, row, col++);

  _date2 = new QDateTimeEdit;
  _date2->setCalendarPopup(TRUE);
  grid->addWidget(_date2, row++, col--);


  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  grid->addWidget(_color, row++, col--);
  _color->setColorButton();

  
  // price
  label = new QLabel(tr("Start Price"));
  grid->addWidget(label, row, col++);

  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  grid->addWidget(_price, row++, col--);


  // price2
  label = new QLabel(tr("End Price"));
  grid->addWidget(label, row, col++);

  _price2 = new QDoubleSpinBox;
  _price2->setRange(0.0, 99999999.0);
  grid->addWidget(_price2, row++, col--);


  // extend
  label = new QLabel(tr("Extend"));
  grid->addWidget(label, row, col++);

  _extend = new QCheckBox;
  grid->addWidget(_extend, row++, col--);


  // default
  label = new QLabel(tr("Set as default"));
  grid->addWidget(label, row, col++);

  _default = new QCheckBox;
  grid->addWidget(_default, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectTLineDialog::done ()
{
  if (_default->isChecked())
  {
    Config config;
    config.setData((int) Config::DefaultChartObjectTLineColor, _color->color());
    config.setData((int) Config::DefaultChartObjectTLineExtend, (int) _extend->isChecked());
  }

  _settings.color = _color->color();
  _settings.date = _date->dateTime();
  _settings.date2 = _date2->dateTime();
  _settings.price = _price->value();
  _settings.price2 = _price2->value();
  _settings.extend = _extend->isChecked();

  emit signalDone(_settings);

  accept();
}

void ChartObjectTLineDialog::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  
  _color->setColor(_settings.color);
  _date->setDateTime(_settings.date);
  _date2->setDateTime(_settings.date2);
  _price->setValue(_settings.price);
  _price2->setValue(_settings.price2);
  _extend->setChecked(_settings.extend);
}

