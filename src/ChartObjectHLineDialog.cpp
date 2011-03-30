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

#include "ChartObjectHLineDialog.h"
#include "Globals.h"

#include <QtDebug>
#include <QSettings>

ChartObjectHLineDialog::ChartObjectHLineDialog (QWidget *p, ChartObject *set) : Dialog (p)
{
  _co = set;
  _keySize = "chart_object_hline_dialog_window_size";
  _keyPos = "chart_object_hline_dialog_window_position";

  QStringList l;
  l << "QtStalker" << g_session << ":" << tr("Edit HLine");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();

  loadObject();
}

void ChartObjectHLineDialog::createGUI ()
{
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

void ChartObjectHLineDialog::done ()
{
  if (_default->isChecked())
  {
    QSettings settings(g_globalSettings);
    settings.setValue("default_chart_object_hline_color", _color->color().name());
    settings.sync();
  }

  Setting *set = _co->settings();
  set->setData("COLOR", _color->color());
  set->setData("PRICE", _price->value());

  _co->save();

  saveSettings();

  accept();
}

void ChartObjectHLineDialog::loadSettings ()
{
  Dialog::loadSettings();

  QSettings settings(g_globalSettings);

  QColor c(settings.value("default_chart_object_hline_color", "red").toString());
  _color->setColor(c);
}

void ChartObjectHLineDialog::loadObject ()
{
  Setting *set = _co->settings();
  _color->setColor(set->color("COLOR"));
  _price->setValue(set->getDouble("PRICE"));
}
