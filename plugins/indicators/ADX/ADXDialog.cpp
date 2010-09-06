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

#include "ADXDialog.h"
#include "FunctionADX.h"
#include "ADX.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
#include <QStringList>

ADXDialog::ADXDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMDIPage();
  createPDIPage();
  createADXPage();
  createADXRPage();
}

void ADXDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  QLabel *label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(ADX::Period));
  grid->addWidget(_period, row++, col--);

  grid->setRowStretch(row, 1);
  
  _tabs->addTab(w, tr("General"));  
}

void ADXDialog::createMDIPage ()
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
  _settings.getData(ADX::MDIColor, d);
  QColor c(d);

  _mdiColor = new ColorButton(this, c);
  _mdiColor->setColorButton();
  grid->addWidget(_mdiColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::MDIPlot, d);

  _mdiPlotStyle = new QComboBox;
  _mdiPlotStyle->addItems(l);
  _mdiPlotStyle->setCurrentIndex(_mdiPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_mdiPlotStyle, row++, col--);

  
  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(ADX::MDILabel, d);

  _mdiLabel = new QLineEdit(d);
  grid->addWidget(_mdiLabel, row++, col--);


  // check
  label = new QLabel(tr("Show"));
  grid->addWidget(label, row, col++);

  _mdiShow = new QCheckBox;
  _mdiShow->setChecked(_settings.getInt(ADX::MDICheck));
  grid->addWidget(_mdiShow, row++, col--);
  

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "-DI");
}

void ADXDialog::createPDIPage ()
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
  _settings.getData(ADX::PDIColor, d);
  QColor c(d);

  _pdiColor = new ColorButton(this, c);
  _pdiColor->setColorButton();
  grid->addWidget(_pdiColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::PDIPlot, d);

  _pdiPlotStyle = new QComboBox;
  _pdiPlotStyle->addItems(l);
  _pdiPlotStyle->setCurrentIndex(_pdiPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_pdiPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(ADX::PDILabel, d);

  _pdiLabel = new QLineEdit(d);
  grid->addWidget(_pdiLabel, row++, col--);


  // check
  label = new QLabel(tr("Show"));
  grid->addWidget(label, row, col++);

  _pdiShow = new QCheckBox;
  _pdiShow->setChecked(_settings.getInt(ADX::PDICheck));
  grid->addWidget(_pdiShow, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "+DI");
}

void ADXDialog::createADXPage ()
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
  _settings.getData(ADX::ADXColor, d);
  QColor c(d);

  _adxColor = new ColorButton(this, c);
  _adxColor->setColorButton();
  grid->addWidget(_adxColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::ADXPlot, d);

  _adxPlotStyle = new QComboBox;
  _adxPlotStyle->addItems(l);
  _adxPlotStyle->setCurrentIndex(_adxPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_adxPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(ADX::ADXLabel, d);

  _adxLabel = new QLineEdit(d);
  grid->addWidget(_adxLabel, row++, col--);


  // check
  label = new QLabel(tr("Show"));
  grid->addWidget(label, row, col++);

  _adxShow = new QCheckBox;
  _adxShow->setChecked(_settings.getInt(ADX::ADXCheck));
  grid->addWidget(_adxShow, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "ADX");
}

void ADXDialog::createADXRPage ()
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
  _settings.getData(ADX::ADXRColor, d);
  QColor c(d);

  _adxrColor = new ColorButton(this, c);
  _adxrColor->setColorButton();
  grid->addWidget(_adxrColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::ADXRPlot, d);

  _adxrPlotStyle = new QComboBox;
  _adxrPlotStyle->addItems(l);
  _adxrPlotStyle->setCurrentIndex(_adxrPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_adxrPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(ADX::ADXRLabel, d);

  _adxrLabel = new QLineEdit(d);
  grid->addWidget(_adxrLabel, row++, col--);


  // check
  label = new QLabel(tr("Show"));
  grid->addWidget(label, row, col++);

  _adxrShow = new QCheckBox;
  _adxrShow->setChecked(_settings.getInt(ADX::ADXRCheck));
  grid->addWidget(_adxrShow, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "ADXR");
}

void ADXDialog::done ()
{
  _settings.setData(ADX::ADXColor, _adxColor->color().name());
  _settings.setData(ADX::ADXRColor, _adxrColor->color().name());
  _settings.setData(ADX::PDIColor, _pdiColor->color().name());
  _settings.setData(ADX::MDIColor, _mdiColor->color().name());
  _settings.setData(ADX::ADXPlot, _adxPlotStyle->currentText());
  _settings.setData(ADX::ADXRPlot, _adxrPlotStyle->currentText());
  _settings.setData(ADX::PDIPlot, _pdiPlotStyle->currentText());
  _settings.setData(ADX::MDIPlot, _mdiPlotStyle->currentText());
  _settings.setData(ADX::ADXLabel, _adxLabel->text());
  _settings.setData(ADX::ADXRLabel, _adxrLabel->text());
  _settings.setData(ADX::PDILabel, _pdiLabel->text());
  _settings.setData(ADX::MDILabel, _mdiLabel->text());
  _settings.setData(ADX::ADXCheck, (int) _adxShow->isChecked());
  _settings.setData(ADX::ADXRCheck, (int) _adxrShow->isChecked());
  _settings.setData(ADX::PDICheck, (int) _pdiShow->isChecked());
  _settings.setData(ADX::MDICheck, (int) _mdiShow->isChecked());
  _settings.setData(ADX::Period, (int) _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.setIndicator(_indicator);

  emit signalDone(_indicator);

  accept();
}

