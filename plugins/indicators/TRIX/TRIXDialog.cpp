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
#include "FunctionTRIX.h"
#include "FunctionMA.h"
#include "TRIX.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

TRIXDialog::TRIXDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void TRIXDialog::createGeneralPage ()
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
  _settings.getData(TRIX::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(1, 100000);
  _period->setValue(_settings.getInt(TRIX::Period));
  grid->addWidget(_period, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(TRIX::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(TRIX::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(TRIX::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void TRIXDialog::done ()
{
  _settings.setData(TRIX::Input, _input->currentText());
  _settings.setData(TRIX::Color, _color->color().name());
  _settings.setData(TRIX::Plot, _plotStyle->currentText());
  _settings.setData(TRIX::Label, _label->text());
  _settings.setData(TRIX::Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}
