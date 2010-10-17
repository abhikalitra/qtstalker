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

#include "SZDialog.h"
#include "FunctionMA.h"
#include "SZ.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

SZDialog::SZDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void SZDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // method
  SZ f;
  QStringList l = f.list();

  QString d;
  _settings.getData(SZ::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(SZ::_Period));
  form->addRow(tr("Period"), _period);

  // color
  _settings.getData(SZ::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(SZ::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(SZ::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // no decline period
  _ndperiod = new QSpinBox;
  _ndperiod->setRange(1, 100000);
  _ndperiod->setValue(_settings.getInt(SZ::_NoDeclinePeriod));
  form->addRow(tr("No Decline Period"), _ndperiod);

  // coefficient
  _coeff = new QDoubleSpinBox;
  _coeff->setRange(0, 100000);
  _coeff->setValue(_settings.getDouble(SZ::_Coefficient));
  form->addRow(tr("Coefficient"), _coeff);

  _tabs->addTab(w, tr("General"));
}

void SZDialog::done ()
{
  _settings.setData(SZ::_Method, _method->currentText());
  _settings.setData(SZ::_Color, _color->color().name());
  _settings.setData(SZ::_Plot, _plotStyle->currentText());
  _settings.setData(SZ::_Label, _label->text());
  _settings.setData(SZ::_NoDeclinePeriod, _ndperiod->value());
  _settings.setData(SZ::_Coefficient, _coeff->value());
  _settings.setData(SZ::_Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

