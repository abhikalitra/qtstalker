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

#include "BOPDialog.h"
#include "FunctionMA.h"
#include "BOP.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

BOPDialog::BOPDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void BOPDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BOP::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BOP::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(BOP::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(BOP::_Smoothing));
  form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(BOP::_SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  _tabs->addTab(w, tr("General"));
}

void BOPDialog::done ()
{
  _settings.setData(BOP::_Color, _color->color().name());
  _settings.setData(BOP::_Plot, _plotStyle->currentText());
  _settings.setData(BOP::_Label, _label->text());
  _settings.setData(BOP::_Smoothing, _smoothing->value());
  _settings.setData(BOP::_SmoothingType, _smoothingType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

