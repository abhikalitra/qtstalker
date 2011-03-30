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
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

ChartObjectRetracementDialog::ChartObjectRetracementDialog (QWidget *p, ChartObject *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_retracement_dialog_window_size";
  _keyPos = "chart_object_retracement_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit Retracement");
  setWindowTitle(l.join(" "));

  createDialog();

  createMainPage();

  createLinePage();

  loadSettings();

  loadObject();
}

void ChartObjectRetracementDialog::createDialog ()
{
  _tabs = new QTabWidget;
  _vbox->insertWidget(0, _tabs);
}

void ChartObjectRetracementDialog::createMainPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  _color = new ColorButton(this, QColor(Qt::red));
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
  _high->setDecimals(4);
  form->addRow(tr("High"), _high);

  // low
  _low = new QDoubleSpinBox;
  _low->setRange(0.0, 99999999.0);
  _low->setDecimals(4);
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
  _line1->setDecimals(4);
  form->addRow(tr("Line 1"), _line1);

  // line2
  _line2 = new QDoubleSpinBox;
  _line2->setRange(0.0, 99999999.0);
  _line2->setDecimals(4);
  form->addRow(tr("Line 2"), _line2);

  // line3
  _line3 = new QDoubleSpinBox;
  _line3->setRange(0.0, 99999999.0);
  _line3->setDecimals(4);
  form->addRow(tr("Line 3"), _line3);

  // line4
  _line4 = new QDoubleSpinBox;
  _line4->setRange(0.0, 99999999.0);
  _line4->setDecimals(4);
  form->addRow(tr("Line 4"), _line4);

  // line5
  _line5 = new QDoubleSpinBox;
  _line5->setRange(0.0, 99999999.0);
  _line5->setDecimals(4);
  form->addRow(tr("Line 5"), _line5);

  // line6
  _line6 = new QDoubleSpinBox;
  _line6->setRange(0.0, 99999999.0);
  _line6->setDecimals(4);
  form->addRow(tr("Line 6"), _line6);

  _tabs->addTab(w, tr("Levels"));
}

void ChartObjectRetracementDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_chart_object_retracement_color", _color->color().name());
    settings.setValue("default_chart_object_retracement_line1", _line1->value());
    settings.setValue("default_chart_object_retracement_line2", _line2->value());
    settings.setValue("default_chart_object_retracement_line3", _line3->value());
    settings.setValue("default_chart_object_retracement_line4", _line4->value());
    settings.setValue("default_chart_object_retracement_line5", _line5->value());
    settings.setValue("default_chart_object_retracement_line6", _line6->value());
    settings.sync();
  }

  Setting *set = _co->settings();
  set->setData("COLOR", _color->color());
  set->setData("DATE", _date->dateTime());
  set->setData("DATE2", _date2->dateTime());
  set->setData("HIGH", _high->value());
  set->setData("LOW", _low->value());
  set->setData("EXTEND", _extend->isChecked());
  set->setData("LINE1", _line1->value());
  set->setData("LINE2", _line2->value());
  set->setData("LINE3", _line3->value());
  set->setData("LINE4", _line4->value());
  set->setData("LINE5", _line5->value());
  set->setData("LINE6", _line6->value());

  _co->save();

  saveSettings();

  accept();
}

void ChartObjectRetracementDialog::loadObject ()
{
  Setting *set = _co->settings();
  _color->setColor(set->color("COLOR"));
  _date->setDateTime(set->dateTime("DATE"));
  _date2->setDateTime(set->dateTime("DATE2"));
  _high->setValue(set->getDouble("HIGH"));
  _low->setValue(set->getDouble("LOW"));
  _extend->setChecked(set->getInt("EXTEND"));
  _line1->setValue(set->getDouble("LINE1"));
  _line2->setValue(set->getDouble("LINE2"));
  _line3->setValue(set->getDouble("LINE3"));
  _line4->setValue(set->getDouble("LINE4"));
  _line5->setValue(set->getDouble("LINE5"));
  _line6->setValue(set->getDouble("LINE6"));
}

void ChartObjectRetracementDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_retracement_color", "red").toString());
  _color->setColor(c);

  _line1->setValue(settings.value("default_chart_object_retracement_line1").toDouble());
  _line2->setValue(settings.value("default_chart_object_retracement_line2").toDouble());
  _line3->setValue(settings.value("default_chart_object_retracement_line3").toDouble());
  _line4->setValue(settings.value("default_chart_object_retracement_line4").toDouble());
  _line5->setValue(settings.value("default_chart_object_retracement_line5").toDouble());
  _line6->setValue(settings.value("default_chart_object_retracement_line6").toDouble());
}
