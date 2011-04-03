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

#include <QtDebug>
#include <QSettings>
#include <QFormLayout>

RetracementDialog::RetracementDialog (QWidget *p, Setting *set) : QWidget (p)
{
  _co = set;
  createGUI();
}

void RetracementDialog::createGUI ()
{
  QVBoxLayout *vbox = new QVBoxLayout;
  setLayout(vbox);

  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  _date->setDateTime(_co->dateTime("DATE"));
  form->addRow(tr("Start Date"), _date);

  // date2
  _date2 = new QDateTimeEdit;
  _date2->setCalendarPopup(TRUE);
  _date2->setDateTime(_co->dateTime("DATE2"));
  form->addRow(tr("End Date"), _date2);

  // color
  _color = new ColorButton(this, QColor(_co->color("COLOR")));
  form->addRow(tr("Color"), _color);
  
  // high
  _high = new QDoubleSpinBox;
  _high->setRange(0.0, 99999999.0);
  _high->setDecimals(4);
  _high->setValue(_co->getDouble("HIGH"));
  form->addRow(tr("High"), _high);

  // low
  _low = new QDoubleSpinBox;
  _low->setRange(0.0, 99999999.0);
  _low->setDecimals(4);
  _low->setValue(_co->getDouble("LOW"));
  form->addRow(tr("Low"), _low);

  // extend
  _extend = new QCheckBox;
  bool d = _co->getInt("EXTEND");
  _extend->setChecked(d);
  form->addRow(tr("Extend"), _extend);

  // default
  _default = new QCheckBox;
  form->addRow(tr("Set as default"), _default);

  // z
  _z = new QSpinBox;
  _z->setRange(-1, 99);
  _z->setValue(_co->getInt("Z"));
  form->addRow(tr("Plot Order"), _z);

  _tabs->addTab(w, tr("Settings"));  
}

void RetracementDialog::createLinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  w->setLayout(form);

  // line1
  _line1 = new QDoubleSpinBox;
  _line1->setRange(0.0, 99999999.0);
  _line1->setDecimals(4);
  _line1->setValue(_co->getDouble("LINE1"));
  form->addRow(tr("Line 1"), _line1);

  // line2
  _line2 = new QDoubleSpinBox;
  _line2->setRange(0.0, 99999999.0);
  _line2->setDecimals(4);
  _line2->setValue(_co->getDouble("LINE2"));
  form->addRow(tr("Line 2"), _line2);

  // line3
  _line3 = new QDoubleSpinBox;
  _line3->setRange(0.0, 99999999.0);
  _line3->setDecimals(4);
  _line3->setValue(_co->getDouble("LINE3"));
  form->addRow(tr("Line 3"), _line3);

  // line4
  _line4 = new QDoubleSpinBox;
  _line4->setRange(0.0, 99999999.0);
  _line4->setDecimals(4);
  _line4->setValue(_co->getDouble("LINE4"));
  form->addRow(tr("Line 4"), _line4);

  // line5
  _line5 = new QDoubleSpinBox;
  _line5->setRange(0.0, 99999999.0);
  _line5->setDecimals(4);
  _line5->setValue(_co->getDouble("LINE5"));
  form->addRow(tr("Line 5"), _line5);

  // line6
  _line6 = new QDoubleSpinBox;
  _line6->setRange(0.0, 99999999.0);
  _line6->setDecimals(4);
  _line6->setValue(_co->getDouble("LINE6"));
  form->addRow(tr("Line 6"), _line6);

  _tabs->addTab(w, tr("Levels"));
}

void RetracementDialog::save ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_retracement_color", _color->color().name());
    settings.setValue("default_retracement_line1", _line1->value());
    settings.setValue("default_retracement_line2", _line2->value());
    settings.setValue("default_retracement_line3", _line3->value());
    settings.setValue("default_retracement_line4", _line4->value());
    settings.setValue("default_retracement_line5", _line5->value());
    settings.setValue("default_retracement_line6", _line6->value());
    settings.sync();
  }

  _co->setData("COLOR", _color->color());
  _co->setData("DATE", _date->dateTime());
  _co->setData("DATE2", _date2->dateTime());
  _co->setData("HIGH", _high->value());
  _co->setData("LOW", _low->value());
  _co->setData("EXTEND", _extend->isChecked());
  _co->setData("LINE1", _line1->value());
  _co->setData("LINE2", _line2->value());
  _co->setData("LINE3", _line3->value());
  _co->setData("LINE4", _line4->value());
  _co->setData("LINE5", _line5->value());
  _co->setData("LINE6", _line6->value());
  _co->setData("Z", _z->text());
}
