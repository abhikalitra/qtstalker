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

#include "TLineDialog.h"
#include "Globals.h"
#include "ChartObjectData.h"

#include <QtDebug>

TLineDialog::TLineDialog (QWidget *p, Data *set) : Dialog (p)
{
  _settings = set;
  _keySize = "tline_dialog_window_size";
  _keyPos = "tline_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Editing TLine Object");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void TLineDialog::createGUI ()
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
  set = _settings->get(ChartObjectData::_DATE2);
  if (set)
    dt = set->toDateTime();

  _date2 = new QDateTimeEdit;
  _date2->setToolTip(tr("End Date"));
  _date2->setCalendarPopup(TRUE);
  _date2->setDisplayFormat("yyyy.MM.dd HH:mm:ss");
  _date2->setDateTime(dt);
  form->addRow(tr("End Date"), _date2);

  // price
  double d = 0;
  set = _settings->get(ChartObjectData::_PRICE);
  if (set)
    d = set->toDouble();

  _price = new QDoubleSpinBox;
  _price->setToolTip(tr("Start Price"));
  _price->setRange(-99999999.0, 99999999.0);
  _price->setValue(d);
  form->addRow(tr("Start Price"), _price);

  // price 2
  d = 0;
  set = _settings->get(ChartObjectData::_PRICE2);
  if (set)
    d = set->toDouble();

  _price2 = new QDoubleSpinBox;
  _price2->setToolTip(tr("End Price"));
  _price2->setRange(-99999999.0, 99999999.0);
  _price2->setValue(d);
  form->addRow(tr("End Price"), _price2);

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

void TLineDialog::done ()
{
  Setting *set = _settings->get(ChartObjectData::_COLOR);
  if (set)
    set->set(_color->color());

  set = _settings->get(ChartObjectData::_DATE);
  if (set)
    set->set(_date->dateTime());

  set = _settings->get(ChartObjectData::_DATE2);
  if (set)
    set->set(_date2->dateTime());

  set = _settings->get(ChartObjectData::_PRICE);
  if (set)
    set->set(_price->value());

  set = _settings->get(ChartObjectData::_PRICE2);
  if (set)
    set->set(_price2->value());

  set = _settings->get(ChartObjectData::_EXTEND);
  if (set)
    set->set(_extend->isChecked());

  set = _settings->get(ChartObjectData::_Z);
  if (set)
    set->set(_z->value());

  saveSettings();

  accept();
}
