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
#include "ADX.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
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

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(ADX::_Period));
  form->addRow(tr("Period"), _period);
  
  _tabs->addTab(w, tr("General"));  
}

void ADXDialog::createMDIPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(ADX::_MDIColor, d);
  QColor c(d);

  _mdiColor = new ColorButton(this, c);
  _mdiColor->setColorButton();
  form->addRow(tr("Color"), _mdiColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::_MDIPlot, d);
  
  _mdiPlotStyle = new QComboBox;
  _mdiPlotStyle->addItems(l);
  _mdiPlotStyle->setCurrentIndex(_mdiPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _mdiPlotStyle);
  
  // label
  _settings.getData(ADX::_MDILabel, d);

  _mdiLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _mdiLabel);

  // check
  _mdiShow = new QCheckBox;
  _mdiShow->setChecked(_settings.getInt(ADX::_MDICheck));
  form->addRow(tr("Show"), _mdiShow);

  _tabs->addTab(w, "-DI");
}

void ADXDialog::createPDIPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(ADX::_PDIColor, d);
  QColor c(d);

  _pdiColor = new ColorButton(this, c);
  _pdiColor->setColorButton();
  form->addRow(tr("Color"), _pdiColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::_PDIPlot, d);

  _pdiPlotStyle = new QComboBox;
  _pdiPlotStyle->addItems(l);
  _pdiPlotStyle->setCurrentIndex(_pdiPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _pdiPlotStyle);

  // label
  _settings.getData(ADX::_PDILabel, d);

  _pdiLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _pdiLabel);

  // check
  _pdiShow = new QCheckBox;
  _pdiShow->setChecked(_settings.getInt(ADX::_PDICheck));
  form->addRow(tr("Show"), _pdiShow);

  _tabs->addTab(w, "+DI");
}

void ADXDialog::createADXPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(ADX::_ADXColor, d);
  QColor c(d);

  _adxColor = new ColorButton(this, c);
  _adxColor->setColorButton();
  form->addRow(tr("Color"), _adxColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::_ADXPlot, d);

  _adxPlotStyle = new QComboBox;
  _adxPlotStyle->addItems(l);
  _adxPlotStyle->setCurrentIndex(_adxPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _adxPlotStyle);

  // label
  _settings.getData(ADX::_ADXLabel, d);

  _adxLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _adxLabel);

  // check
  _adxShow = new QCheckBox;
  _adxShow->setChecked(_settings.getInt(ADX::_ADXCheck));
  form->addRow(tr("Show"), _adxShow);

  _tabs->addTab(w, "ADX");
}

void ADXDialog::createADXRPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(ADX::_ADXRColor, d);
  QColor c(d);

  _adxrColor = new ColorButton(this, c);
  _adxrColor->setColorButton();
  form->addRow(tr("Color"), _adxrColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(ADX::_ADXRPlot, d);

  _adxrPlotStyle = new QComboBox;
  _adxrPlotStyle->addItems(l);
  _adxrPlotStyle->setCurrentIndex(_adxrPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _adxrPlotStyle);

  // label
  _settings.getData(ADX::_ADXRLabel, d);

  _adxrLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _adxrLabel);

  // check
  _adxrShow = new QCheckBox;
  _adxrShow->setChecked(_settings.getInt(ADX::_ADXRCheck));
  form->addRow(tr("Show"), _adxrShow);

  _tabs->addTab(w, "ADXR");
}

void ADXDialog::done ()
{
  _settings.setData(ADX::_ADXColor, _adxColor->color().name());
  _settings.setData(ADX::_ADXRColor, _adxrColor->color().name());
  _settings.setData(ADX::_PDIColor, _pdiColor->color().name());
  _settings.setData(ADX::_MDIColor, _mdiColor->color().name());
  _settings.setData(ADX::_ADXPlot, _adxPlotStyle->currentText());
  _settings.setData(ADX::_ADXRPlot, _adxrPlotStyle->currentText());
  _settings.setData(ADX::_PDIPlot, _pdiPlotStyle->currentText());
  _settings.setData(ADX::_MDIPlot, _mdiPlotStyle->currentText());
  _settings.setData(ADX::_ADXLabel, _adxLabel->text());
  _settings.setData(ADX::_ADXRLabel, _adxrLabel->text());
  _settings.setData(ADX::_PDILabel, _pdiLabel->text());
  _settings.setData(ADX::_MDILabel, _mdiLabel->text());
  _settings.setData(ADX::_ADXCheck, (int) _adxShow->isChecked());
  _settings.setData(ADX::_ADXRCheck, (int) _adxrShow->isChecked());
  _settings.setData(ADX::_PDICheck, (int) _pdiShow->isChecked());
  _settings.setData(ADX::_MDICheck, (int) _mdiShow->isChecked());
  _settings.setData(ADX::_Period, (int) _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

