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

#include "TRIXDialog.h"
#include "FunctionMA.h"
#include "TRIX.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

TRIXDialog::TRIXDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void TRIXDialog::createGeneralPage ()
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
  _settings.getData(TRIX::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(TRIX::_Period));
  form->addRow(tr("Period"), _period);

  // color
  _settings.getData(TRIX::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(TRIX::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(TRIX::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  _tabs->addTab(w, tr("General"));
}

void TRIXDialog::done ()
{
  _settings.setData(TRIX::_Input, _input->currentText());
  _settings.setData(TRIX::_Color, _color->color().name());
  _settings.setData(TRIX::_Plot, _plotStyle->currentText());
  _settings.setData(TRIX::_Label, _label->text());
  _settings.setData(TRIX::_Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

