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
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
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

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // input
  BarData bd;
  QStringList l;
  bd.getInputFields(l);

  QString d;
  _settings.getData(BBANDS::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(BBANDS::_Period));
  form->addRow(tr("Period"), _period);

  // up deviation
  _upDev = new QDoubleSpinBox;
  _upDev->setRange(-100000, 100000);
  _upDev->setValue(_settings.getDouble(BBANDS::_UpDeviation));
  form->addRow(tr("Upper Deviation"), _upDev);

  // low deviation
  _lowDev = new QDoubleSpinBox;
  _lowDev->setRange(-100000, 100000);
  _lowDev->setValue(_settings.getDouble(BBANDS::_DownDeviation));
  form->addRow(tr("Lower Deviation"), _lowDev);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(BBANDS::_MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _maType);

  _tabs->addTab(w, tr("General"));
}

void BBANDSDialog::createUpperPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BBANDS::_UpColor, d);
  QColor c(d);

  _upperColor = new ColorButton(this, c);
  _upperColor->setColorButton();
  form->addRow(tr("Color"), _upperColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::_UpPlot, d);

  _upperPlotStyle = new QComboBox;
  _upperPlotStyle->addItems(l);
  _upperPlotStyle->setCurrentIndex(_upperPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _upperPlotStyle);

  // label
  _settings.getData(BBANDS::_UpLabel, d);

  _upperLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _upperLabel);

  _tabs->addTab(w, tr("Upper"));
}

void BBANDSDialog::createMiddlePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BBANDS::_MidColor, d);
  QColor c(d);

  _midColor = new ColorButton(this, c);
  _midColor->setColorButton();
  form->addRow(tr("Color"), _midColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::_MidPlot, d);

  _midPlotStyle = new QComboBox;
  _midPlotStyle->addItems(l);
  _midPlotStyle->setCurrentIndex(_midPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _midPlotStyle);

  // label
  _settings.getData(BBANDS::_MidLabel, d);

  _midLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _midLabel);

  _tabs->addTab(w, tr("Middle"));
}

void BBANDSDialog::createLowerPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BBANDS::_DownColor, d);
  QColor c(d);

  _lowerColor = new ColorButton(this, c);
  _lowerColor->setColorButton();
  form->addRow(tr("Color"), _lowerColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BBANDS::_DownPlot, d);

  _lowerPlotStyle = new QComboBox;
  _lowerPlotStyle->addItems(l);
  _lowerPlotStyle->setCurrentIndex(_lowerPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _lowerPlotStyle);

  // label
  _settings.getData(BBANDS::_DownLabel, d);

  _lowerLabel = new QLineEdit(d);
  form->addRow(tr("Color"), _lowerLabel);

  _tabs->addTab(w, tr("Lower"));
}

void BBANDSDialog::createBarsPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  QString d;
  _settings.getData(BBANDS::_UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _settings.getData(BBANDS::_DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _settings.getData(BBANDS::_BarsNeutralColor, d);
  c.setNamedColor(d);

  _neutralColor = new ColorButton(this, c);
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // label
  _settings.getData(BBANDS::_BarsLabel, d);

  _barsLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _barsLabel);

  _tabs->addTab(w, tr("BARS"));
}

void BBANDSDialog::done ()
{
  _settings.setData(BBANDS::_UpColor, _upperColor->color().name());
  _settings.setData(BBANDS::_MidColor, _midColor->color().name());
  _settings.setData(BBANDS::_DownColor, _lowerColor->color().name());
  _settings.setData(BBANDS::_UpPlot, _upperPlotStyle->currentText());
  _settings.setData(BBANDS::_MidPlot, _midPlotStyle->currentText());
  _settings.setData(BBANDS::_DownPlot, _lowerPlotStyle->currentText());
  _settings.setData(BBANDS::_UpLabel, _upperLabel->text());
  _settings.setData(BBANDS::_MidLabel, _midLabel->text());
  _settings.setData(BBANDS::_DownLabel, _lowerLabel->text());
  _settings.setData(BBANDS::_UpDeviation, _upDev->value());
  _settings.setData(BBANDS::_DownDeviation, _lowDev->value());
  _settings.setData(BBANDS::_Input, _input->currentText());
  _settings.setData(BBANDS::_Period, _period->value());
  _settings.setData(BBANDS::_MAType, _maType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

