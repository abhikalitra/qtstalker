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
#include <QLayout>
#include <QLabel>
#include <QStringList>

THERMDialog::THERMDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMAPage();
}

void THERMDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // up color
  QLabel *label = new QLabel(tr("Up Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(THERM::UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  grid->addWidget(_upColor, row++, col--);


  // down color
  label = new QLabel(tr("Down Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(THERM::DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  grid->addWidget(_downColor, row++, col--);


  // thresh color
  label = new QLabel(tr("Threshold Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(THERM::ThreshColor, d);
  c.setNamedColor(d);

  _threshColor = new ColorButton(this, c);
  _threshColor->setColorButton();
  grid->addWidget(_threshColor, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(THERM::Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // threshold
  label = new QLabel(tr("Threshold"));
  grid->addWidget(label, row, col++);

  _threshold = new QDoubleSpinBox;
  _threshold->setRange(0, 100000);
  _threshold->setValue(_settings.getDouble(THERM::Threshold));
  grid->addWidget(_threshold, row++, col--);

  // smoothing
  label = new QLabel(tr("Smoothing"));
  grid->addWidget(label, row, col++);

  _smoothing = new QSpinBox;
  _smoothing->setRange(1, 100000);
  _smoothing->setValue(_settings.getInt(THERM::Smoothing));
  grid->addWidget(_smoothing, row++, col--);

  // smoothing type
  label = new QLabel(tr("Smoothing Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  QStringList l = mau.list();

  _settings.getData(THERM::SmoothingType, d);

  _smoothingType = new QComboBox;
  _smoothingType->addItems(l);
  _smoothingType->setCurrentIndex(_smoothingType->findText(d, Qt::MatchExactly));
  grid->addWidget(_smoothingType, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("THERM"));
}

void THERMDialog::createMAPage ()
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
  _settings.getData(THERM::MAColor, d);
  QColor c(d);

  _maColor = new ColorButton(this, c);
  _maColor->setColorButton();
  grid->addWidget(_maColor, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(THERM::MAPlot, d);

  _maPlotStyle = new QComboBox;
  _maPlotStyle->addItems(l);
  _maPlotStyle->setCurrentIndex(_maPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_maPlotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(THERM::MALabel, d);

  _maLabel = new QLineEdit(d);
  grid->addWidget(_maLabel, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 100000);
  _maPeriod->setValue(_settings.getInt(THERM::MAPeriod));
  grid->addWidget(_maPeriod, row++, col--);

  // ma type
  label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(THERM::MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  grid->addWidget(_maType, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("MA"));
}

void THERMDialog::done ()
{
  _settings.setData(THERM::UpColor, _upColor->color().name());
  _settings.setData(THERM::DownColor, _downColor->color().name());
  _settings.setData(THERM::ThreshColor, _threshColor->color().name());
  _settings.setData(THERM::Label, _label->text());
  _settings.setData(THERM::MAColor, _maColor->color().name());
  _settings.setData(THERM::MAPlot, _maPlotStyle->currentText());
  _settings.setData(THERM::MALabel, _maLabel->text());
  _settings.setData(THERM::MAPeriod, _maPeriod->value());
  _settings.setData(THERM::MAType, _maType->currentText());
  _settings.setData(THERM::Threshold, _threshold->value());
  _settings.setData(THERM::Smoothing, _smoothing->value());
  _settings.setData(THERM::SmoothingType, _smoothingType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}

