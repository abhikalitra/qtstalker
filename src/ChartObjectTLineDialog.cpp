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
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

ChartObjectTLineDialog::ChartObjectTLineDialog (QWidget *p, ChartObject *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_tline_dialog_window_size";
  _keyPos = "chart_object_tline_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit TLine");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadObject();
}

void ChartObjectTLineDialog::createGUI ()
{
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  _form->addRow(tr("Start Date"), _date);

  // date2
  _date2 = new QDateTimeEdit;
  _date2->setCalendarPopup(TRUE);
  _form->addRow(tr("End Date"), _date2);

  // color
  _color = new ColorButton(this, QColor(Qt::red));
  _form->addRow(tr("Color"), _color);
  
  // price
  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  _price->setDecimals(4);
  _form->addRow(tr("Start Price"), _price);

  // price2
  _price2 = new QDoubleSpinBox;
  _price2->setRange(0.0, 99999999.0);
  _price2->setDecimals(4);
  _form->addRow(tr("End Price"), _price2);

  // extend
  _extend = new QCheckBox;
  _form->addRow(tr("Extend"), _extend);

  // default
  _default = new QCheckBox;
  _form->addRow(tr("Set as default"), _default);
}

void ChartObjectTLineDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_chart_object_tline_color", _color->color().name());
    settings.setValue("default_chart_object_tline_extend", _extend->isChecked());
    settings.sync();
  }

  Setting *set = _co->settings();
  set->setData("COLOR", _color->color());
  set->setData("DATE", _date->dateTime());
  set->setData("DATE2", _date2->dateTime());
  set->setData("PRICE", _price->value());
  set->setData("PRICE2", _price2->value());
  set->setData("EXTEND", _extend->isChecked());

  _co->save();

  saveSettings();

  accept();
}

void ChartObjectTLineDialog::loadObject ()
{
  Setting *set = _co->settings();
  _date->setDateTime(set->dateTime("DATE"));
  _date2->setDateTime(set->dateTime("DATE2"));
  _color->setColor(set->color("COLOR"));
  _price->setValue(set->getDouble("PRICE"));
  _price2->setValue(set->getDouble("PRICE2"));
  _extend->setChecked(set->getInt("EXTEND"));
}

void ChartObjectTLineDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_tline_color", "red").toString());
  _color->setColor(c);

  _extend->setChecked(settings.value("default_chart_object_tline_extend", FALSE).toBool());
}
