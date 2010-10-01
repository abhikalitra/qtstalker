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

#include "MACDDialog.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "MACD.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

MACDDialog::MACDDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMACDPage();
  createSignalPage();
  createHistPage();
}

void MACDDialog::createGeneralPage ()
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
  _settings.getData(MACD::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // fast
  label = new QLabel(tr("Fast Period"));
  grid->addWidget(label, row, col++);

  _fast = new QSpinBox;
  _fast->setRange(2, 100000);
  _fast->setValue(_settings.getInt(MACD::_FastPeriod));
  grid->addWidget(_fast, row++, col--);

  // slow
  label = new QLabel(tr("Slow Period"));
  grid->addWidget(label, row, col++);

  _slow = new QSpinBox;
  _slow->setRange(2, 100000);
  _slow->setValue(_settings.getInt(MACD::_SlowPeriod));
  grid->addWidget(_slow, row++, col--);

  // fast ma
  label = new QLabel(tr("Fast MA"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(MACD::_FastMA, d);

  _fastMAType = new QComboBox;
  _fastMAType->addItems(l);
  _fastMAType->setCurrentIndex(_fastMAType->findText(d, Qt::MatchExactly));
  grid->addWidget(_fastMAType, row++, col--);

  // slow ma
  label = new QLabel(tr("Slow MA"));
  grid->addWidget(label, row, col++);

  _settings.getData(MACD::_SlowMA, d);

  _slowMAType = new QComboBox;
  _slowMAType->addItems(l);
  _slowMAType->setCurrentIndex(_slowMAType->findText(d, Qt::MatchExactly));
  grid->addWidget(_slowMAType, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void MACDDialog::createMACDPage ()
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
  _settings.getData(MACD::_MACDColor, d);
  QColor c(d);

  _macdColor = new ColorButton(this, c);
  _macdColor->setColorButton();
  grid->addWidget(_macdColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MACD::_MACDPlot, d);

  _macdPlotStyle = new QComboBox;
  _macdPlotStyle->addItems(l);
  _macdPlotStyle->setCurrentIndex(_macdPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_macdPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MACD::_MACDLabel, d);

  _macdLabel = new QLineEdit(d);
  grid->addWidget(_macdLabel, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("MACD"));
}

void MACDDialog::createSignalPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // ma type
  QLabel *label = new QLabel(tr("MA Type"));
  grid->addWidget(label, row, col++);

  FunctionMA f;
  QStringList l = f.list();

  QString d;
  _settings.getData(MACD::_SignalMA, d);

  _signalMAType = new QComboBox;
  _signalMAType->addItems(l);
  _signalMAType->setCurrentIndex(_signalMAType->findText(d, Qt::MatchExactly));
  grid->addWidget(_signalMAType, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _signalPeriod = new QSpinBox;
  _signalPeriod->setRange(1, 100000);
  _signalPeriod->setValue(_settings.getInt(MACD::_SignalPeriod));
  grid->addWidget(_signalPeriod, row++, col--);

  // color
  label = new QLabel(tr("Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(MACD::_SignalColor, d);
  QColor c(d);

  _signalColor = new ColorButton(this, c);
  _signalColor->setColorButton();
  grid->addWidget(_signalColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(MACD::_SignalPlot, d);

  _signalPlotStyle = new QComboBox;
  _signalPlotStyle->addItems(l);
  _signalPlotStyle->setCurrentIndex(_signalPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_signalPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MACD::_SignalLabel, d);

  _signalLabel = new QLineEdit(d);
  grid->addWidget(_signalLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Signal"));
}

void MACDDialog::createHistPage ()
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
  _settings.getData(MACD::_HistColor, d);
  QColor c(d);

  _histColor = new ColorButton(this, c);
  _histColor->setColorButton();
  grid->addWidget(_histColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MACD::_HistPlot, d);

  _histPlotStyle = new QComboBox;
  _histPlotStyle->addItems(l);
  _histPlotStyle->setCurrentIndex(_histPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_histPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MACD::_HistLabel, d);

  _histLabel = new QLineEdit(d);
  grid->addWidget(_histLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Histogram"));
}

void MACDDialog::done ()
{
  _settings.setData(MACD::_MACDColor, _macdColor->color().name());
  _settings.setData(MACD::_SignalColor, _signalColor->color().name());
  _settings.setData(MACD::_HistColor, _histColor->color().name());
  _settings.setData(MACD::_MACDPlot, _macdPlotStyle->currentText());
  _settings.setData(MACD::_SignalPlot, _signalPlotStyle->currentText());
  _settings.setData(MACD::_HistPlot, _histPlotStyle->currentText());
  _settings.setData(MACD::_MACDLabel, _macdLabel->text());
  _settings.setData(MACD::_SignalLabel, _signalLabel->text());
  _settings.setData(MACD::_HistLabel, _histLabel->text());
  _settings.setData(MACD::_FastPeriod, _fast->value());
  _settings.setData(MACD::_SlowPeriod, _slow->value());
  _settings.setData(MACD::_SignalPeriod, _signalPeriod->value());
  _settings.setData(MACD::_FastMA, _fastMAType->currentText());
  _settings.setData(MACD::_SlowMA, _slowMAType->currentText());
  _settings.setData(MACD::_SignalMA, _signalMAType->currentText());
  _settings.setData(MACD::_Input, _input->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

