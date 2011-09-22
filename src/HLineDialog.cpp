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

#include "HLineDialog.h"
#include "Globals.h"
#include "ChartObjectData.h"

#include <QtDebug>

HLineDialog::HLineDialog (QWidget *p, Data *set) : Dialog (p)
{
  _settings = set;
  _keySize = "hline_dialog_window_size";
  _keyPos = "hline_dialog_window_position";

  QStringList l;
  l << "QtStalker" + g_session + ":" << tr("Editing HLine Object");
  setWindowTitle(l.join(" "));

  createGUI();

  loadSettings();
}

void HLineDialog::createGUI ()
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

  // price
  double d = 0;
  set = _settings->get(ChartObjectData::_PRICE);
  if (set)
    d = set->toDouble();

  _price = new QDoubleSpinBox;
  _price->setToolTip(tr("Price"));
  _price->setRange(-99999999.0, 99999999.0);
  _price->setValue(d);
  form->addRow(tr("Price"), _price);

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

void HLineDialog::done ()
{
  Setting *set = _settings->get(ChartObjectData::_COLOR);
  if (set)
    set->set(_color->color());

  set = _settings->get(ChartObjectData::_PRICE);
  if (set)
    set->set(_price->value());

  set = _settings->get(ChartObjectData::_Z);
  if (set)
    set->set(_z->value());

  saveSettings();

  accept();
}
