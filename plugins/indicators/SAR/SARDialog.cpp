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

#include "SARDialog.h"
#include "FunctionMA.h"
#include "SAR.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

SARDialog::SARDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void SARDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(SAR::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // label
  _settings.getData(SAR::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // init
  _init = new QDoubleSpinBox;
  _init->setRange(0, 0.2);
  _init->setValue(_settings.getDouble(SAR::_Init));
  form->addRow(tr("Initial"), _init);

  // max
  _max = new QDoubleSpinBox;
  _max->setRange(0, 0.2);
  _max->setValue(_settings.getDouble(SAR::_Max));
  form->addRow(tr("Max"), _max);

  _tabs->addTab(w, tr("SAR"));
}

void SARDialog::done ()
{
  _settings.setData(SAR::_Color, _color->color().name());
  _settings.setData(SAR::_Label, _label->text());
  _settings.setData(SAR::_Init, _init->value());
  _settings.setData(SAR::_Max, _max->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

