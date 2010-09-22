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

#include "LINEARREGDialog.h"
#include "FunctionLINEARREG.h"
#include "LINEARREG.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

LINEARREGDialog::LINEARREGDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
}

void LINEARREGDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // method
  QLabel *label = new QLabel(tr("Method"));
  grid->addWidget(label, row, col++);

  FunctionLINEARREG f;
  QStringList l = f.list();

  QString d;
  _settings.getData(LINEARREG::Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  grid->addWidget(_method, row++, col--);


  // input
  label = new QLabel(tr("Input"));
  grid->addWidget(label, row, col++);

  BarData bd;
  bd.getInputFields(l);

  _settings.getData(LINEARREG::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(LINEARREG::Period));
  grid->addWidget(_period, row++, col--);


  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(LINEARREG::Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(LINEARREG::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(LINEARREG::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void LINEARREGDialog::done ()
{
  _settings.setData(LINEARREG::Method, _method->currentText());
  _settings.setData(LINEARREG::Period, _period->value());
  _settings.setData(LINEARREG::Color, _color->color().name());
  _settings.setData(LINEARREG::Plot, _plotStyle->currentText());
  _settings.setData(LINEARREG::Label, _label->text());
  _settings.setData(LINEARREG::Input, _input->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

