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

#include "BBANDSDialog.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "BBANDS.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

BBANDSDialog::BBANDSDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createBarsPage();
  createUpperPage();
  createMiddlePage();
  createLowerPage();
}

void BBANDSDialog::createGeneralPage ()
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
  _settings.getData(BBANDS::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(BBANDS::Period));
  grid->addWidget(_period, row++, col--);

  // up deviation
  label = new QLabel(tr("Upper Deviation"));
  grid->addWidget(label, row, col++);

  _upDev = new QDoubleSpinBox;
  _upDev->setRange(-100000, 100000);
  _upDev->setValue(_settings.getDouble(BBANDS::UpDeviation));
  grid->addWidget(_upDev, row++, col--);

  // low deviation
  label = new QLabel(tr("Lower Deviation"));
  grid->addWidget(label, row, col++);

  _lowDev = new QDoubleSpinBox;
  _lowDev->setRange(-100000, 100000);
  _lowDev->setValue(_settings.getDouble(BBANDS::DownDeviation));
  grid->addWidget(_lowDev, row++, col--);

  // ma type
  label = new QLabel(tr("MA Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(BBANDS::MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  grid->addWidget(_maType, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void BBANDSDialog::createUpperPage ()
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
  _settings.getData(BBANDS::UpColor, d);
  QColor c(d);

  _upperColor = new ColorButton(this, c);
  _upperColor->setColorButton();
  grid->addWidget(_upperColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::UpPlot, d);

  _upperPlotStyle = new QComboBox;
  _upperPlotStyle->addItems(l);
  _upperPlotStyle->setCurrentIndex(_upperPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_upperPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::UpLabel, d);

  _upperLabel = new QLineEdit(d);
  grid->addWidget(_upperLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Upper"));
}

void BBANDSDialog::createMiddlePage ()
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
  _settings.getData(BBANDS::MidColor, d);
  QColor c(d);

  _midColor = new ColorButton(this, c);
  _midColor->setColorButton();
  grid->addWidget(_midColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::MidPlot, d);

  _midPlotStyle = new QComboBox;
  _midPlotStyle->addItems(l);
  _midPlotStyle->setCurrentIndex(_midPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_midPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::MidLabel, d);

  _midLabel = new QLineEdit(d);
  grid->addWidget(_midLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Middle"));
}

void BBANDSDialog::createLowerPage ()
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
  _settings.getData(BBANDS::DownColor, d);
  QColor c(d);

  _lowerColor = new ColorButton(this, c);
  _lowerColor->setColorButton();
  grid->addWidget(_lowerColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::DownPlot, d);

  _lowerPlotStyle = new QComboBox;
  _lowerPlotStyle->addItems(l);
  _lowerPlotStyle->setCurrentIndex(_lowerPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_lowerPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::DownLabel, d);

  _lowerLabel = new QLineEdit(d);
  grid->addWidget(_lowerLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Lower"));
}

void BBANDSDialog::createBarsPage ()
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
  _settings.getData(BBANDS::UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  grid->addWidget(_upColor, row++, col--);


  // down color
  label = new QLabel(tr("Down Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  grid->addWidget(_downColor, row++, col--);


  // neutral color
  label = new QLabel(tr("Neutral Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::BarsNeutralColor, d);
  c.setNamedColor(d);

  _neutralColor = new ColorButton(this, c);
  _neutralColor->setColorButton();
  grid->addWidget(_neutralColor, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(BBANDS::BarsLabel, d);

  _barsLabel = new QLineEdit(d);
  grid->addWidget(_barsLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("BARS"));
}

void BBANDSDialog::done ()
{
  _settings.setData(BBANDS::UpColor, _upperColor->color().name());
  _settings.setData(BBANDS::MidColor, _midColor->color().name());
  _settings.setData(BBANDS::DownColor, _lowerColor->color().name());
  _settings.setData(BBANDS::UpPlot, _upperPlotStyle->currentText());
  _settings.setData(BBANDS::MidPlot, _midPlotStyle->currentText());
  _settings.setData(BBANDS::DownPlot, _lowerPlotStyle->currentText());
  _settings.setData(BBANDS::UpLabel, _upperLabel->text());
  _settings.setData(BBANDS::MidLabel, _midLabel->text());
  _settings.setData(BBANDS::DownLabel, _lowerLabel->text());
  _settings.setData(BBANDS::UpDeviation, _upDev->value());
  _settings.setData(BBANDS::DownDeviation, _lowDev->value());
  _settings.setData(BBANDS::Input, _input->currentText());
  _settings.setData(BBANDS::Period, _period->value());
  _settings.setData(BBANDS::MAType, _maType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}

