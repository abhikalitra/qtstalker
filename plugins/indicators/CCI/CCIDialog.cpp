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

#include "CCIDialog.h"
#include "FunctionMA.h"
#include "CCI.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

CCIDialog::CCIDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createRefPage();
  createRef2Page();
}

void CCIDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CCI::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CCI::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(CCI::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(CCI::_Period));
  form->addRow(tr("Period"), _period);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(CCI::_Smoothing));
  form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(CCI::_SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  _tabs->addTab(w, tr("General"));
}

void CCIDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CCI::_Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(CCI::_Ref1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, tr("Ref 1"));
}

void CCIDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CCI::_Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(CCI::_Ref2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, tr("Ref 2"));
}

void CCIDialog::done ()
{
  _settings.setData(CCI::_Color, _color->color().name());
  _settings.setData(CCI::_Plot, _plotStyle->currentText());
  _settings.setData(CCI::_Label, _label->text());
  _settings.setData(CCI::_Smoothing, _smoothing->value());
  _settings.setData(CCI::_SmoothingType, _smoothingType->currentText());
  _settings.setData(CCI::_Period, _period->value());
  _settings.setData(CCI::_Ref1, _ref->value());
  _settings.setData(CCI::_Ref2, _ref2->value());
  _settings.setData(CCI::_Ref1Color, _refColor->color().name());
  _settings.setData(CCI::_Ref2Color, _ref2Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

