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

#include "PODialog.h"
#include "FunctionMA.h"
#include "PO.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

PODialog::PODialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void PODialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // method
  PO f;
  QStringList l = f.list();

  QString d;
  _settings.getData(PO::_Method, d);
  
  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);
  
  // input
  BarData bd;
  bd.getInputFields(l);

  _settings.getData(PO::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // fast
  _fast = new QSpinBox;
  _fast->setRange(2, 100000);
  _fast->setValue(_settings.getInt(PO::_FastPeriod));
  form->addRow(tr("Fast Period"), _fast);

  // slow
  _slow = new QSpinBox;
  _slow->setRange(2, 100000);
  _slow->setValue(_settings.getInt(PO::_SlowPeriod));
  form->addRow(tr("Slow Period"), _slow);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(PO::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  // color
  _settings.getData(PO::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(PO::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(PO::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  _tabs->addTab(w, tr("General"));
}

void PODialog::done ()
{
  _settings.setData(PO::_Method, _method->currentText());
  _settings.setData(PO::_Input, _input->currentText());
  _settings.setData(PO::_Color, _color->color().name());
  _settings.setData(PO::_Plot, _plotStyle->currentText());
  _settings.setData(PO::_Label, _label->text());
  _settings.setData(PO::_MAType, _maType->currentText());
  _settings.setData(PO::_FastPeriod, _fast->value());
  _settings.setData(PO::_SlowPeriod, _slow->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

