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
#include <QLayout>
#include <QLabel>
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

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QColor c(Qt::red);
  _color = new ColorButton(this, c);
  grid->addWidget(_color, row++, col--);
  _color->setColorButton();

  
  // date
  label = new QLabel(tr("Start Date"));
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

  
  // high
  label = new QLabel(tr("High"));
  grid->addWidget(label, row, col++);

  _high = new QDoubleSpinBox;
  _high->setRange(0.0, 99999999.0);
  _high->setValue(0);
  grid->addWidget(_high, row++, col--);


  // low
  label = new QLabel(tr("Low"));
  grid->addWidget(label, row, col++);

  _low = new QDoubleSpinBox;
  _low->setRange(0.0, 99999999.0);
  _low->setValue(0);
  grid->addWidget(_low, row++, col--);


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

void ChartObjectRetracementDialog::createLinePage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // line1
  QLabel *label = new QLabel(tr("Line 1"));
  grid->addWidget(label, row, col++);

  _line1 = new QDoubleSpinBox;
  _line1->setRange(0.0, 99999999.0);
  grid->addWidget(_line1, row++, col--);


  // line2
  label = new QLabel(tr("Line 2"));
  grid->addWidget(label, row, col++);

  _line2 = new QDoubleSpinBox;
  _line2->setRange(0.0, 99999999.0);
  grid->addWidget(_line2, row++, col--);


  // line3
  label = new QLabel(tr("Line 3"));
  grid->addWidget(label, row, col++);

  _line3 = new QDoubleSpinBox;
  _line3->setRange(0.0, 99999999.0);
  grid->addWidget(_line3, row++, col--);


  // line4
  label = new QLabel(tr("Line 4"));
  grid->addWidget(label, row, col++);

  _line4 = new QDoubleSpinBox;
  _line4->setRange(0.0, 99999999.0);
  grid->addWidget(_line4, row++, col--);


  // line5
  label = new QLabel(tr("Line 5"));
  grid->addWidget(label, row, col++);

  _line5 = new QDoubleSpinBox;
  _line5->setRange(0.0, 99999999.0);
  grid->addWidget(_line5, row++, col--);


  // line6
  label = new QLabel(tr("Line 6"));
  grid->addWidget(label, row, col++);

  _line6 = new QDoubleSpinBox;
  _line6->setRange(0.0, 99999999.0);
  grid->addWidget(_line6, row++, col--);

  grid->setRowStretch(row, 1);

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

