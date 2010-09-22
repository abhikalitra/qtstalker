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

#include "MOMDialog.h"
#include "FunctionMOM.h"
#include "FunctionMA.h"
#include "MOM.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

MOMDialog::MOMDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void MOMDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // input
  QLabel *label = new QLabel(tr("Input"));
  grid->addWidget(label, row, col++);

  BarData bd;
  QStringList l;
  bd.getInputFields(l);

  QString d;
  _settings.getData(MOM::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(MOM::Period));
  grid->addWidget(_period, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(MOM::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(MOM::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MOM::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // smoothing
  label = new QLabel(tr("Smoothing"));
  grid->addWidget(label, row, col++);

  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(MOM::Smoothing));
  grid->addWidget(_smoothing, row++, col--);

  // smoothing type
  label = new QLabel(tr("Smoothing Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(MOM::SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  grid->addWidget(_smoothingType, row++, col--);
  
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void MOMDialog::done ()
{
  _settings.setData(MOM::Input, _input->currentText());
  _settings.setData(MOM::Color, _color->color().name());
  _settings.setData(MOM::Plot, _plotStyle->currentText());
  _settings.setData(MOM::Label, _label->text());
  _settings.setData(MOM::Smoothing, _smoothing->value());
  _settings.setData(MOM::SmoothingType, _smoothingType->currentText());
  _settings.setData(MOM::Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

