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

#include "BETADialog.h"
#include "BETA.h"
#include "Curve.h"
#include "ExchangeDataBase.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

BETADialog::BETADialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void BETADialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BETA::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BETA::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(BETA::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // period
  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(BETA::_Period));
  form->addRow(tr("Period"), _period);

  // input
  BarData bd;
  bd.getInputFields(l);

  _settings.getData(BETA::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // exchange
  ExchangeDataBase db;
  db.getExchanges(l);

  _settings.getData(BETA::_Exchange, d);

  _exchange = new QComboBox;
  _exchange->addItems(l);
  _exchange->setCurrentIndex(_exchange->findText(d, Qt::MatchExactly));
  form->addRow(tr("Index Exchange"), _exchange);

  // input2
  _settings.getData(BETA::_Index, d);

  _input2 = new QLineEdit(d);
  _input2->setToolTip(tr("Index symbol used for comparison eg. SP500"));
  form->addRow(tr("Index Symbol"), _input2);

  _tabs->addTab(w, tr("General"));
}

void BETADialog::done ()
{
  _settings.setData(BETA::_Index, _input2->text());
  _settings.setData(BETA::_Exchange, _exchange->currentText());
  _settings.setData(BETA::_Color, _color->color().name());
  _settings.setData(BETA::_Plot, _plotStyle->currentText());
  _settings.setData(BETA::_Label, _label->text());
  _settings.setData(BETA::_Input, _input->currentText());
  _settings.setData(BETA::_Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

