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

#include "ChartObjectRetracementDialog.h"
#include "Config.h"
#include "ChartObject.h"
#include "Globals.h"

#include <QtDebug>
#include <QFormLayout>
#include <QString>

ChartObjectRetracementDialog::ChartObjectRetracementDialog ()
{
  setWindowTitle("Qtstalker" + g_session + ": " + tr("Edit Retracement Chart Object"));

  createMainPage();
  createLinePage();
}

void ChartObjectRetracementDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);
  
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  form->addRow(tr("Start Date"), _date);

  // date2
  _date2 = new QDateTimeEdit;
  _date2->setCalendarPopup(TRUE);
  form->addRow(tr("End Date"), _date2);
  
  // high
  _high = new QDoubleSpinBox;
  _high->setRange(0.0, 99999999.0);
  _high->setValue(0);
  form->addRow(tr("High"), _high);

  // low
  _low = new QDoubleSpinBox;
  _low->setRange(0.0, 99999999.0);
  _low->setValue(0);
  form->addRow(tr("Low"), _low);

  // extend
  _extend = new QCheckBox;
  form->addRow(tr("Extend"), _extend);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  _tabs->addTab(w, tr("Settings"));
}

void ChartObjectRetracementDialog::createLinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // line1
  _line1 = new QDoubleSpinBox;
  _line1->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 1"), _line1);

  // line2
  _line2 = new QDoubleSpinBox;
  _line2->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 2"), _line2);

  // line3
  _line3 = new QDoubleSpinBox;
  _line3->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 3"), _line3);

  // line4
  _line4 = new QDoubleSpinBox;
  _line4->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 4"), _line4);

  // line5
  _line5 = new QDoubleSpinBox;
  _line5->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 5"), _line5);

  // line6
  _line6 = new QDoubleSpinBox;
  _line6->setRange(0.0, 99999999.0);
  form->addRow(tr("Line 6"), _line6);

  _tabs->addTab(w, tr("Levels"));
}

void ChartObjectRetracementDialog::done ()
{
  if (_default->isChecked())
  {
    Config config;
    config.transaction();
    config.setData(Config::DefaultChartObjectRetracementColor, _color->color());
    config.setData(Config::DefaultChartObjectRetracementLine1, _line1->value());
    config.setData(Config::DefaultChartObjectRetracementLine2, _line2->value());
    config.setData(Config::DefaultChartObjectRetracementLine3, _line3->value());
    config.setData(Config::DefaultChartObjectRetracementLine4, _line4->value());
    config.setData(Config::DefaultChartObjectRetracementLine5, _line5->value());
    config.setData(Config::DefaultChartObjectRetracementLine6, _line6->value());
    config.commit();
  }

  _settings.color = _color->color();
  _settings.date = _date->dateTime();
  _settings.date2 = _date2->dateTime();
  _settings.high = _high->value();
  _settings.low = _low->value();
  _settings.extend = _extend->isChecked();
  _settings.line1 = _line1->value();
  _settings.line2 = _line2->value();
  _settings.line3 = _line3->value();
  _settings.line4 = _line4->value();
  _settings.line5 = _line5->value();
  _settings.line6 = _line6->value();

  emit signalDone(_settings);

  accept();
}

void ChartObjectRetracementDialog::setSettings (ChartObjectSettings &d)
{
  _settings = d;
  
  _color->setColor(_settings.color);
  _date->setDateTime(_settings.date);
  _date2->setDateTime(_settings.date2);
  _high->setValue(_settings.high);
  _low->setValue(_settings.low);
  _line1->setValue(_settings.line1);
  _line2->setValue(_settings.line2);
  _line3->setValue(_settings.line3);
  _line4->setValue(_settings.line4);
  _line5->setValue(_settings.line5);
  _line6->setValue(_settings.line6);
  _extend->setChecked(_settings.extend);
}

