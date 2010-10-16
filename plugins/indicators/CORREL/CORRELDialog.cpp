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

#include "CORRELDialog.h"
#include "CORREL.h"
#include "Curve.h"
#include "ExchangeDataBase.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

CORRELDialog::CORRELDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createRefPage();
  createRef2Page();
  createRef3Page();
}

void CORRELDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CORREL::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CORREL::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(CORREL::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(CORREL::_Period));
  form->addRow(tr("Period"), _period);

  // input
  BarData bd;
  bd.getInputFields(l);

  _settings.getData(CORREL::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // exchange
  ExchangeDataBase db;
  db.getExchanges(l);

  _settings.getData(CORREL::_Exchange, d);

  _exchange = new QComboBox;
  _exchange->addItems(l);
  _exchange->setCurrentIndex(_exchange->findText(d, Qt::MatchExactly));
  form->addRow(tr("Index Exchange"), _exchange);

  // input2
  _settings.getData(CORREL::_Input2, d);

  _input2 = new QLineEdit(d);
  _input2->setToolTip(tr("Index symbol used for comparison eg. SP500"));
  form->addRow(tr("Index Symbol"), _input2);

  _tabs->addTab(w, tr("General"));
}

void CORRELDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CORREL::_Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(CORREL::_Ref1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, tr("Ref 1"));
}

void CORRELDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CORREL::_Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(CORREL::_Ref2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, tr("Ref 2"));
}

void CORRELDialog::createRef3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CORREL::_Ref3Color, d);
  QColor c(d);

  _ref3Color = new ColorButton(this, c);
  _ref3Color->setColorButton();
  form->addRow(tr("Color"), _ref3Color);

  // ref
  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(-100000, 100000);
  _ref3->setValue(_settings.getDouble(CORREL::_Ref3));
  form->addRow(tr("Value"), _ref3);

  _tabs->addTab(w, tr("Ref 3"));
}

void CORRELDialog::done ()
{
  _settings.setData(CORREL::_Input2, _input2->text());
  _settings.setData(CORREL::_Exchange, _exchange->currentText());
  _settings.setData(CORREL::_Color, _color->color().name());
  _settings.setData(CORREL::_Plot, _plotStyle->currentText());
  _settings.setData(CORREL::_Label, _label->text());
  _settings.setData(CORREL::_Input, _input->currentText());
  _settings.setData(CORREL::_Period, _period->value());
  _settings.setData(CORREL::_Ref1, _ref->value());
  _settings.setData(CORREL::_Ref2, _ref2->value());
  _settings.setData(CORREL::_Ref3, _ref3->value());
  _settings.setData(CORREL::_Ref1Color, _refColor->color().name());
  _settings.setData(CORREL::_Ref2Color, _ref2Color->color().name());
  _settings.setData(CORREL::_Ref3Color, _ref3Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

