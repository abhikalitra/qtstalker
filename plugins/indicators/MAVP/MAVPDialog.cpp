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

#include "MAVPDialog.h"
#include "FunctionMA.h"
#include "MAVP.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

MAVPDialog::MAVPDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void MAVPDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // input
  BarData bd;
  QStringList l;
  bd.getInputFields(l);

  QString d;
  _settings.getData(MAVP::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // input2
  _settings.getData(MAVP::_Input2, d);

  _input2 = new QComboBox;
  _input2->addItems(l);
  _input2->setCurrentIndex(_input2->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input 2"), _input2);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(MAVP::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  // min
  _min = new QSpinBox;
  _min->setRange(2, 100000);
  _min->setValue(_settings.getInt(MAVP::_Min));
  form->addRow(tr("Min"), _min);

  // max
  _max = new QSpinBox;
  _max->setRange(2, 100000);
  _max->setValue(_settings.getInt(MAVP::_Max));
  form->addRow(tr("Max"), _max);

  // color
  _settings.getData(MAVP::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(MAVP::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(MAVP::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  _tabs->addTab(w, tr("General"));
}

void MAVPDialog::done ()
{
  _settings.setData(MAVP::_MAType, _maType->currentText());
  _settings.setData(MAVP::_Input, _input->currentText());
  _settings.setData(MAVP::_Input2, _input2->currentText());
  _settings.setData(MAVP::_Min, _min->value());
  _settings.setData(MAVP::_Max, _max->value());
  _settings.setData(MAVP::_Color, _color->color().name());
  _settings.setData(MAVP::_Plot, _plotStyle->currentText());
  _settings.setData(MAVP::_Label, _label->text());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

