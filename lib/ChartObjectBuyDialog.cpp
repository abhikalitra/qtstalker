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

#include "ChartObjectBuyDialog.h"
#include "Config.h"
#include "ChartObject.h"
#include "Globals.h"

#include <QtDebug>
#include <QString>
#include <QFormLayout>

ChartObjectBuyDialog::ChartObjectBuyDialog ()
{
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Edit Buy Chart Object"));

  createMainPage();
}

void ChartObjectBuyDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  form->addRow(tr("Date"), _date);

  // color
  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);
  
  // price
  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  form->addRow(tr("Price"), _price);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectBuyDialog::done ()
{
  if (_default->isChecked())
  {
    Config config;
    config.transaction();
    config.setData((int) Config::DefaultChartObjectBuyColor, _color->color());
    config.commit();
  }

  _settings.color = _color->color();
  _settings.date = _date->dateTime();
  _settings.price = _price->value();
  
  emit signalDone(_settings);

  accept();
}

void ChartObjectBuyDialog::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  
  _color->setColor(_settings.color);
  _date->setDateTime(_settings.date);
  _price->setValue(_settings.price);
}

