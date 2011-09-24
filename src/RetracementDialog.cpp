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

#include "RetracementDialog.h"
#include "Globals.h"
#include "ChartObjectData.h"

#include <QtDebug>

RetracementDialog::RetracementDialog (QWidget *p, Data *set) : Dialog (p)
{
  _settings = set;
  _keySize = "retracement_dialog_window_size";
  _keyPos = "retracement_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Editing Retracement Object");
  setWindowTitle(l.join(" "));

  createGUI();
  createGUI2();

  loadSettings();
}

void RetracementDialog::createGUI ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(5);
  form->setMargin(10);
  w->setLayout(form);

  // color
  QColor c(Qt::green);
  Setting *set = _settings->get(ChartObjectData::_COLOR);
  if (set)
    c = set->toColor();

  _color = new ColorButton(w, c);
  _color->setToolTip(tr("Color"));
  form->addRow(tr("Color"), _color);

  // start date
  QDateTime dt = QDateTime::currentDateTime();
  set = _settings->get(ChartObjectData::_DATE);
  if (set)
    dt = set->toDateTime();

  _date = new QDateTimeEdit;
  _date->setToolTip(tr("Start Date"));
  _date->setCalendarPopup(TRUE);
  _date->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  _date->setDateTime(dt);
  form->addRow(tr("Start Date"), _date);

  // end date
  dt = QDateTime::currentDateTime();
  set = _settings->get(ChartObjectData::_DATE_2);
  if (set)
    dt = set->toDateTime();

  _date2 = new QDateTimeEdit;
  _date2->setToolTip(tr("End Date"));
  _date2->setCalendarPopup(TRUE);
  _date2->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  _date2->setDateTime(dt);
  form->addRow(tr("End Date"), _date2);

  // high
  double d = 0;
  set = _settings->get(ChartObjectData::_HIGH);
  if (set)
    d = set->toDouble();

  _high = new QDoubleSpinBox;
  _high->setToolTip(tr("High"));
  _high->setRange(-99999999.0, 99999999.0);
  _high->setValue(d);
  form->addRow(tr("High"), _high);

  // low
  d = 0;
  set = _settings->get(ChartObjectData::_LOW);
  if (set)
    d = set->toDouble();

  _low = new QDoubleSpinBox;
  _low->setToolTip(tr("Low"));
  _low->setRange(-99999999.0, 99999999.0);
  _low->setValue(d);
  form->addRow(tr("Low"), _low);

  // extend
  bool b = FALSE;
  set = _settings->get(ChartObjectData::_EXTEND);
  if (set)
    b = set->toBool();

  _extend = new QCheckBox;
  _extend->setToolTip(tr("Extend"));
  _extend->setChecked(b);
  form->addRow(tr("Extend"), _extend);

  // z
  int z = 1;
  set = _settings->get(ChartObjectData::_Z);
  if (set)
    z = set->toInteger();

  _z = new QSpinBox;
  _z->setToolTip(tr("Z Level"));
  _z->setRange(1, 99);
  _z->setValue(z);
  form->addRow(tr("Z Level"), _z);

  // clear room
  _message->hide();

  _tabs->addTab(w, tr("Settings"));
}

void RetracementDialog::createGUI2 ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(5);
  form->setMargin(10);
  w->setLayout(form);

  // level 1
  double d = 0;
  Setting *set = _settings->get(ChartObjectData::_LEVEL_1);
  if (set)
    d = set->toDouble();

  _level1 = new QDoubleSpinBox;
  _level1->setToolTip(tr("Level 1"));
  _level1->setRange(-99999999.0, 99999999.0);
  _level1->setValue(d);
  form->addRow(tr("Level 1"), _level1);

  // level 2
  d = 0;
  set = _settings->get(ChartObjectData::_LEVEL_2);
  if (set)
    d = set->toDouble();

  _level2 = new QDoubleSpinBox;
  _level2->setToolTip(tr("Level 2"));
  _level2->setRange(-99999999.0, 99999999.0);
  _level2->setValue(d);
  form->addRow(tr("Level 2"), _level2);

  // level 3
  d = 0;
  set = _settings->get(ChartObjectData::_LEVEL_3);
  if (set)
    d = set->toDouble();

  _level3 = new QDoubleSpinBox;
  _level3->setToolTip(tr("Level 3"));
  _level3->setRange(-99999999.0, 99999999.0);
  _level3->setValue(d);
  form->addRow(tr("Level 3"), _level3);

  // level 4
  d = 0;
  set = _settings->get(ChartObjectData::_LEVEL_4);
  if (set)
    d = set->toDouble();

  _level4 = new QDoubleSpinBox;
  _level4->setToolTip(tr("Level 4"));
  _level4->setRange(-99999999.0, 99999999.0);
  _level4->setValue(d);
  form->addRow(tr("Level 4"), _level4);

  // level 5
  d = 0;
  set = _settings->get(ChartObjectData::_LEVEL_5);
  if (set)
    d = set->toDouble();

  _level5 = new QDoubleSpinBox;
  _level5->setToolTip(tr("Level 5"));
  _level5->setRange(-99999999.0, 99999999.0);
  _level5->setValue(d);
  form->addRow(tr("Level 5"), _level5);

  // level 6
  d = 0;
  set = _settings->get(ChartObjectData::_LEVEL_6);
  if (set)
    d = set->toDouble();

  _level6 = new QDoubleSpinBox;
  _level6->setToolTip(tr("Level 6"));
  _level6->setRange(-99999999.0, 99999999.0);
  _level6->setValue(d);
  form->addRow(tr("Level 6"), _level6);

  _tabs->addTab(w, tr("Levels"));
}

void RetracementDialog::done ()
{
  Setting *set = _settings->get(ChartObjectData::_COLOR);
  if (set)
    set->set(_color->color());

  set = _settings->get(ChartObjectData::_DATE);
  if (set)
    set->set(_date->dateTime());

  set = _settings->get(ChartObjectData::_DATE_2);
  if (set)
    set->set(_date2->dateTime());

  set = _settings->get(ChartObjectData::_HIGH);
  if (set)
    set->set(_high->value());

  set = _settings->get(ChartObjectData::_LOW);
  if (set)
    set->set(_low->value());

  set = _settings->get(ChartObjectData::_EXTEND);
  if (set)
    set->set(_extend->isChecked());

  set = _settings->get(ChartObjectData::_Z);
  if (set)
    set->set(_z->value());

  set = _settings->get(ChartObjectData::_LEVEL_1);
  if (set)
    set->set(_level1->value());

  set = _settings->get(ChartObjectData::_LEVEL_2);
  if (set)
    set->set(_level2->value());

  set = _settings->get(ChartObjectData::_LEVEL_3);
  if (set)
    set->set(_level3->value());

  set = _settings->get(ChartObjectData::_LEVEL_4);
  if (set)
    set->set(_level4->value());

  set = _settings->get(ChartObjectData::_LEVEL_5);
  if (set)
    set->set(_level5->value());

  set = _settings->get(ChartObjectData::_LEVEL_6);
  if (set)
    set->set(_level6->value());

  saveSettings();

  accept();
}
