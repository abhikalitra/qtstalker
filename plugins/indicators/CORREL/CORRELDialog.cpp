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
#include "FunctionCORREL.h"
#include "CORREL.h"
#include "Curve.h"
#include "ExchangeDataBase.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
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
  _settings.getData(CORREL::Color, d);
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

  _settings.getData(CORREL::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CORREL::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(CORREL::Period));
  grid->addWidget(_period, row++, col--);

  // input
  label = new QLabel(tr("Input"));
  grid->addWidget(label, row, col++);

  BarData bd;
  bd.getInputFields(l);

  _settings.getData(CORREL::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // exchange
  label = new QLabel(tr("Index Exchange"));
  grid->addWidget(label, row, col++);

  ExchangeDataBase db;
  db.getExchanges(l);

  _settings.getData(CORREL::Exchange, d);

  _exchange = new QComboBox;
  _exchange->addItems(l);
  _exchange->setCurrentIndex(_exchange->findText(d, Qt::MatchExactly));
  grid->addWidget(_exchange, row++, col--);

  // input2
  label = new QLabel(tr("Index Symbol"));
  grid->addWidget(label, row, col++);

  _settings.getData(CORREL::Input2, d);

  _input2 = new QLineEdit(d);
  _input2->setToolTip(tr("Index symbol used for comparison eg. SP500"));
  grid->addWidget(_input2, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void CORRELDialog::createRefPage ()
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
  _settings.getData(CORREL::Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  grid->addWidget(_refColor, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(CORREL::Ref1));
  grid->addWidget(_ref, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 1"));
}

void CORRELDialog::createRef2Page ()
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
  _settings.getData(CORREL::Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  grid->addWidget(_ref2Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(CORREL::Ref2));
  grid->addWidget(_ref2, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 2"));
}

void CORRELDialog::createRef3Page ()
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
  _settings.getData(CORREL::Ref3Color, d);
  QColor c(d);

  _ref3Color = new ColorButton(this, c);
  _ref3Color->setColorButton();
  grid->addWidget(_ref3Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(-100000, 100000);
  _ref3->setValue(_settings.getDouble(CORREL::Ref3));
  grid->addWidget(_ref3, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 3"));
}

void CORRELDialog::done ()
{
  _settings.setData(CORREL::Input2, _input2->text());
  _settings.setData(CORREL::Exchange, _exchange->currentText());
  _settings.setData(CORREL::Color, _color->color().name());
  _settings.setData(CORREL::Plot, _plotStyle->currentText());
  _settings.setData(CORREL::Label, _label->text());
  _settings.setData(CORREL::Input, _input->currentText());
  _settings.setData(CORREL::Period, _period->value());
  _settings.setData(CORREL::Ref1, _ref->value());
  _settings.setData(CORREL::Ref2, _ref2->value());
  _settings.setData(CORREL::Ref3, _ref3->value());
  _settings.setData(CORREL::Ref1Color, _refColor->color().name());
  _settings.setData(CORREL::Ref2Color, _ref2Color->color().name());
  _settings.setData(CORREL::Ref3Color, _ref3Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}
