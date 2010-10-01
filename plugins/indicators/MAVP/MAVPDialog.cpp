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

#include "MAVPDialog.h"
#include "FunctionMA.h"
#include "MAVP.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

MAVPDialog::MAVPDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void MAVPDialog::createGeneralPage ()
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
  _settings.getData(MAVP::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // input2
  label = new QLabel(tr("Input 2"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAVP::_Input2, d);

  _input2 = new QComboBox;
  _input2->addItems(l);
  _input2->setCurrentIndex(_input2->findText(d, Qt::MatchExactly));
  grid->addWidget(_input2, row++, col--);

  // ma type
  label = new QLabel(tr("MA Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(MAVP::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  grid->addWidget(_maType, row++, col--);

  // min
  label = new QLabel(tr("Min"));
  grid->addWidget(label, row, col++);

  _min = new QSpinBox;
  _min->setRange(2, 100000);
  _min->setValue(_settings.getInt(MAVP::_Min));
  grid->addWidget(_min, row++, col--);

  // max
  label = new QLabel(tr("Max"));
  grid->addWidget(label, row, col++);

  _max = new QSpinBox;
  _max->setRange(2, 100000);
  _max->setValue(_settings.getInt(MAVP::_Max));
  grid->addWidget(_max, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAVP::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(MAVP::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAVP::_Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void MAVPDialog::done ()
{
  _settings.setData(MAVP::_MAType, _maType->currentText());
  _settings.setData(MAVP::_Input, _input->currentText());
  _settings.setData(MAVP::_Input2, _input2->currentText());
  _settings.setData(MAVP::_Min, _min->value());
  _settings.setData(MAVP::_Max, _max->value());
  _settings.setData(MAVP::_Color, _color->color().name());
  _settings.setData(MAVP::_Plot, _plotStyle->currentText());
  _settings.setData(MAVP::_Label, _label->text());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

