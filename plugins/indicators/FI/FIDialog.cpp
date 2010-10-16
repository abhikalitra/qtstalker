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

#include "FIDialog.h"
#include "FunctionMA.h"
#include "FI.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

FIDialog::FIDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void FIDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(FI::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(FI::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(FI::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(FI::_Period));
  form->addRow(tr("Period"), _period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(FI::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  _tabs->addTab(w, tr("General"));
}

void FIDialog::done ()
{
  _settings.setData(FI::_MAType, _maType->currentText());
  _settings.setData(FI::_Period, _period->value());
  _settings.setData(FI::_Color, _color->color().name());
  _settings.setData(FI::_Plot, _plotStyle->currentText());
  _settings.setData(FI::_Label, _label->text());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

