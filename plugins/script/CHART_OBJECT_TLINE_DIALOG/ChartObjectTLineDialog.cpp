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
#include "ChartObjectDataBase.h"

#include <QtDebug>

ChartObjectTLineDialog::ChartObjectTLineDialog (QWidget *p, Command *c) : Dialog (p)
{
  _command = c;
  _keySize = "chart_object_tline_dialog_window_size";
  _keyPos = "chart_object_tline_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit TLine") << _command->parm("ID");
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

  _co.setData("Color", _color->color());
  _co.setData("Date", _date->dateTime());
  _co.setData("Date2", _date2->dateTime());
  _co.setData("Price", _price->value());
  _co.setData("Price2", _price2->value());
  _co.setData("Extend", _extend->isChecked());

  ChartObjectDataBase db;
  db.save(&_co);

  saveSettings();

  _command->setReturnCode("0");

  accept();
}

void ChartObjectTLineDialog::loadObject ()
{
  _co.setData("ID", _command->parm("ID"));

  ChartObjectDataBase db;
  db.load(&_co);

  _date->setDateTime(_co.dateTime("Date"));
  _date2->setDateTime(_co.dateTime("Date2"));
  _color->setColor(_co.color("Color"));
  _price->setValue(_co.getDouble("Price"));
  _price2->setValue(_co.getDouble("Price2"));
  _extend->setChecked(_co.getInt("Extend"));
}

void ChartObjectTLineDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_tline_color", "red").toString());
  _color->setColor(c);

  _extend->setChecked(settings.value("default_chart_object_tline_extend", FALSE).toBool());
}
