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

#include "ROCDialog.h"
#include "FunctionMA.h"
#include "ROC.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

ROCDialog::ROCDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void ROCDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // method
  ROC f;
  QStringList l = f.list();

  QString d;
  _settings.getData(ROC::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);

  // input
  BarData bd;
  bd.getInputFields(l);

  _settings.getData(ROC::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(ROC::_Period));
  form->addRow(tr("Period"), _period);

  // color
  _settings.getData(ROC::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(ROC::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(ROC::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(ROC::_Smoothing));
  form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(ROC::_SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  _tabs->addTab(w, tr("General"));
}

void ROCDialog::done ()
{
  _settings.setData(ROC::_Method, _method->currentText());
  _settings.setData(ROC::_Input, _input->currentText());
  _settings.setData(ROC::_Color, _color->color().name());
  _settings.setData(ROC::_Plot, _plotStyle->currentText());
  _settings.setData(ROC::_Label, _label->text());
  _settings.setData(ROC::_Smoothing, _smoothing->value());
  _settings.setData(ROC::_SmoothingType, _smoothingType->currentText());
  _settings.setData(ROC::_Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

