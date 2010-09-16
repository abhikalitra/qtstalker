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
#include "FunctionCCI.h"
#include "FunctionMA.h"
#include "CCI.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
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

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(CCI::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CCI::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CCI::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(CCI::Period));
  grid->addWidget(_period, row++, col--);

  // smoothing
  label = new QLabel(tr("Smoothing"));
  grid->addWidget(label, row, col++);

  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(CCI::Smoothing));
  grid->addWidget(_smoothing, row++, col--);

  // smoothing type
  label = new QLabel(tr("Smoothing Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(CCI::SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  grid->addWidget(_smoothingType, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void CCIDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(CCI::Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  grid->addWidget(_refColor, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(CCI::Ref1));
  grid->addWidget(_ref, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 1"));
}

void CCIDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // color
  QLabel *label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(CCI::Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  grid->addWidget(_ref2Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(CCI::Ref2));
  grid->addWidget(_ref2, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 2"));
}

void CCIDialog::done ()
{
  _settings.setData(CCI::Color, _color->color().name());
  _settings.setData(CCI::Plot, _plotStyle->currentText());
  _settings.setData(CCI::Label, _label->text());
  _settings.setData(CCI::Smoothing, _smoothing->value());
  _settings.setData(CCI::SmoothingType, _smoothingType->currentText());
  _settings.setData(CCI::Period, _period->value());
  _settings.setData(CCI::Ref1, _ref->value());
  _settings.setData(CCI::Ref2, _ref2->value());
  _settings.setData(CCI::Ref1Color, _refColor->color().name());
  _settings.setData(CCI::Ref2Color, _ref2Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}
