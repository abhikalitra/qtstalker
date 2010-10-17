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

#include "THERMDialog.h"
#include "THERM.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

THERMDialog::THERMDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMAPage();
}

void THERMDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  QString d;
  _settings.getData(THERM::_UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _settings.getData(THERM::_DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // thresh color
  _settings.getData(THERM::_ThreshColor, d);
  c.setNamedColor(d);

  _threshColor = new ColorButton(this, c);
  _threshColor->setColorButton();
  form->addRow(tr("Threshold Color"), _threshColor);

  // label
  _settings.getData(THERM::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // threshold
  _threshold = new QDoubleSpinBox;
  _threshold->setRange(0, 100000);
  _threshold->setValue(_settings.getDouble(THERM::_Threshold));
  form->addRow(tr("Threshold"), _threshold);

  // smoothing
  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(THERM::_Smoothing));
  form->addRow(tr("Smoothing"), _smoothing);

  // smoothing type
  FunctionMA mau;
  QStringList l = mau.list();

  _settings.getData(THERM::_SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Smoothing Type"), _smoothingType);

  _tabs->addTab(w, tr("THERM"));
}

void THERMDialog::createMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(THERM::_MAColor, d);
  QColor c(d);

  _maColor = new ColorButton(this, c);
  _maColor->setColorButton();
  form->addRow(tr("Up Color"), _maColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(THERM::_MAPlot, d);

  _maPlotStyle = new QComboBox;
  _maPlotStyle->addItems(l);
  _maPlotStyle->setCurrentIndex(_maPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _maPlotStyle);

  // label
  _settings.getData(THERM::_MALabel, d);

  _maLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _maLabel);

  // period
  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 100000);
  _maPeriod->setValue(_settings.getInt(THERM::_MAPeriod));
  form->addRow(tr("Period"), _maPeriod);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(THERM::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  _tabs->addTab(w, tr("MA"));
}

void THERMDialog::done ()
{
  _settings.setData(THERM::_UpColor, _upColor->color().name());
  _settings.setData(THERM::_DownColor, _downColor->color().name());
  _settings.setData(THERM::_ThreshColor, _threshColor->color().name());
  _settings.setData(THERM::_Label, _label->text());
  _settings.setData(THERM::_MAColor, _maColor->color().name());
  _settings.setData(THERM::_MAPlot, _maPlotStyle->currentText());
  _settings.setData(THERM::_MALabel, _maLabel->text());
  _settings.setData(THERM::_MAPeriod, _maPeriod->value());
  _settings.setData(THERM::_MAType, _maType->currentText());
  _settings.setData(THERM::_Threshold, _threshold->value());
  _settings.setData(THERM::_Smoothing, _smoothing->value());
  _settings.setData(THERM::_SmoothingType, _smoothingType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

