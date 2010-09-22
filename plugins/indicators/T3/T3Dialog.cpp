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

#include "T3Dialog.h"
#include "FunctionT3.h"
#include "FunctionMA.h"
#include "T3.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

T3Dialog::T3Dialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void T3Dialog::createGeneralPage ()
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
  _settings.getData(T3::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(T3::Period));
  grid->addWidget(_period, row++, col--);

  // v factor
  label = new QLabel(tr("V Factor"));
  grid->addWidget(label, row, col++);

  _vfactor = new QDoubleSpinBox;
  _vfactor->setRange(0, 1);
  _vfactor->setValue(_settings.getDouble(T3::VFactor));
  grid->addWidget(_vfactor, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(T3::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(T3::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(T3::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);
 
  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void T3Dialog::done ()
{
  _settings.setData(T3::Input, _input->currentText());
  _settings.setData(T3::Color, _color->color().name());
  _settings.setData(T3::Plot, _plotStyle->currentText());
  _settings.setData(T3::Label, _label->text());
  _settings.setData(T3::VFactor, _vfactor->value());
  _settings.setData(T3::Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

