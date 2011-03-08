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
#include "Globals.h"
#include "ChartObjectDataBase.h"

#include <QtDebug>

ChartObjectBuyDialog::ChartObjectBuyDialog (QWidget *p, Setting *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_buy_dialog_window_size";
  _keyPos = "chart_object_buy_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit Buy");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadObject();
}

void ChartObjectBuyDialog::createGUI ()
{
  // date
  _date = new QDateTimeEdit;
  _date->setCalendarPopup(TRUE);
  _form->addRow(tr("Date"), _date);

  // color
  _color = new ColorButton(this, QColor(Qt::red));
  _form->addRow(tr("Color"), _color);
  
  // price
  _price = new QDoubleSpinBox;
  _price->setRange(0.0, 99999999.0);
  _price->setDecimals(4);
  _form->addRow(tr("Price"), _price);

  // default
  _default = new QCheckBox;
  _form->addRow(tr("Set as default"), _default);
}

void ChartObjectBuyDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_chart_object_buy_color", _color->color().name());
    settings.sync();
  }

  _co->setData("Color", _color->color());
  _co->setData("Price", _price->value());
  _co->setData("Date", _date->dateTime());

  ChartObjectDataBase db;
  db.save(_co);

  saveSettings();

  accept();
}

void ChartObjectBuyDialog::loadObject ()
{
  _color->setColor(_co->color("Color"));
  _date->setDateTime(_co->dateTime("Date"));
  _price->setValue(_co->getDouble("Price"));
}

void ChartObjectBuyDialog::loadSettings ()
{
  Dialog::loadSettings();
  
  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_buy_color", "green").toString());
  _color->setColor(c);
}
