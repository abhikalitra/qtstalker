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

#include "ULTOSCDialog.h"
#include "FunctionMA.h"
#include "ULTOSC.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

ULTOSCDialog::ULTOSCDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createRefPage();
  createRef2Page();
  createRef3Page();
}

void ULTOSCDialog::createGeneralPage ()
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
  _settings.getData(ULTOSC::_Color, d);
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

  _settings.getData(ULTOSC::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(ULTOSC::_Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // short period
  label = new QLabel(tr("Short Period"));
  grid->addWidget(label, row, col++);

  _speriod = new QSpinBox;
  _speriod->setRange(1, 100000);
  _speriod->setValue(_settings.getInt(ULTOSC::_ShortPeriod));
  grid->addWidget(_speriod, row++, col--);

  // med period
  label = new QLabel(tr("Medium Period"));
  grid->addWidget(label, row, col++);

  _mperiod = new QSpinBox;
  _mperiod->setRange(1, 100000);
  _mperiod->setValue(_settings.getInt(ULTOSC::_MidPeriod));
  grid->addWidget(_mperiod, row++, col--);

  // long period
  label = new QLabel(tr("Long Period"));
  grid->addWidget(label, row, col++);

  _lperiod = new QSpinBox;
  _lperiod->setRange(1, 100000);
  _lperiod->setValue(_settings.getInt(ULTOSC::_LongPeriod));
  grid->addWidget(_lperiod, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void ULTOSCDialog::createRefPage ()
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
  _settings.getData(ULTOSC::_Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  grid->addWidget(_refColor, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref = new QDoubleSpinBox;
  _ref->setRange(0, 100);
  _ref->setValue(_settings.getDouble(ULTOSC::_Ref1));
  grid->addWidget(_ref, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 1"));
}

void ULTOSCDialog::createRef2Page ()
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
  _settings.getData(ULTOSC::_Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  grid->addWidget(_ref2Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(0, 100);
  _ref2->setValue(_settings.getDouble(ULTOSC::_Ref2));
  grid->addWidget(_ref2, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 2"));
}

void ULTOSCDialog::createRef3Page ()
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
  _settings.getData(ULTOSC::_Ref3Color, d);
  QColor c(d);

  _ref3Color = new ColorButton(this, c);
  _ref3Color->setColorButton();
  grid->addWidget(_ref3Color, row++, col--);

  // ref
  label = new QLabel(tr("Value"));
  grid->addWidget(label, row, col++);

  _ref3 = new QDoubleSpinBox;
  _ref3->setRange(0, 100);
  _ref3->setValue(_settings.getDouble(ULTOSC::_Ref3));
  grid->addWidget(_ref3, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Ref 3"));
}

void ULTOSCDialog::done ()
{
  _settings.setData(ULTOSC::_Color, _color->color().name());
  _settings.setData(ULTOSC::_Plot, _plotStyle->currentText());
  _settings.setData(ULTOSC::_Label, _label->text());
  _settings.setData(ULTOSC::_ShortPeriod, _speriod->value());
  _settings.setData(ULTOSC::_MidPeriod, _mperiod->value());
  _settings.setData(ULTOSC::_LongPeriod, _lperiod->value());
  _settings.setData(ULTOSC::_Ref1, _ref->value());
  _settings.setData(ULTOSC::_Ref2, _ref2->value());
  _settings.setData(ULTOSC::_Ref3, _ref3->value());
  _settings.setData(ULTOSC::_Ref1Color, _refColor->color().name());
  _settings.setData(ULTOSC::_Ref2Color, _ref2Color->color().name());
  _settings.setData(ULTOSC::_Ref3Color, _ref3Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

