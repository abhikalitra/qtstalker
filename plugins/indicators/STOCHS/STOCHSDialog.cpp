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

#include "STOCHSDialog.h"
#include "FunctionMA.h"
#include "STOCHS.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

STOCHSDialog::STOCHSDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createKPage();
  createDPage();
  createRefPage();
  createRef2Page();
}

void STOCHSDialog::createKPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // ma type
  FunctionMA mau;
  QStringList l = mau.list();

  QString d;
  _settings.getData(STOCHS::_SlowKMA, d);

  _kmaType = new QComboBox;
  _kmaType->addItems(l);
  _kmaType->setCurrentIndex(_kmaType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _kmaType);

  // fast k period
  _fkperiod = new QSpinBox;
  _fkperiod->setRange(1, 100000);
  _fkperiod->setValue(_settings.getInt(STOCHS::_FastKPeriod));
  form->addRow(tr("Fast K Period"), _fkperiod);

  // slow k period
  _kperiod = new QSpinBox;
  _kperiod->setRange(1, 100000);
  _kperiod->setValue(_settings.getInt(STOCHS::_SlowKPeriod));
  form->addRow(tr("Slow K Period"), _kperiod);

  // color
  _settings.getData(STOCHS::_SlowKColor, d);
  QColor c(d);

  _kcolor = new ColorButton(this, c);
  _kcolor->setColorButton();
  form->addRow(tr("Color"), _kcolor);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(STOCHS::_SlowKPlot, d);

  _kplotStyle = new QComboBox;
  _kplotStyle->addItems(l);
  _kplotStyle->setCurrentIndex(_kplotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _kplotStyle);

  // label
  _settings.getData(STOCHS::_SlowKLabel, d);

  _klabel = new QLineEdit(d);
  form->addRow(tr("Label"), _klabel);

  _tabs->addTab(w, "%K");
}

void STOCHSDialog::createDPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // ma type
  FunctionMA mau;
  QStringList l = mau.list();

  QString d;
  _settings.getData(STOCHS::_SlowDMA, d);

  _dmaType = new QComboBox;
  _dmaType->addItems(l);
  _dmaType->setCurrentIndex(_dmaType->findText(d, Qt::MatchExactly));
  form->addRow(tr("MA Type"), _dmaType);

  // period
  _dperiod = new QSpinBox;
  _dperiod->setRange(1, 100000);
  _dperiod->setValue(_settings.getInt(STOCHS::_SlowDPeriod));
  form->addRow(tr("Period"), _dperiod);

  // color
  _settings.getData(STOCHS::_SlowDColor, d);
  QColor c(d);

  _dcolor = new ColorButton(this, c);
  _dcolor->setColorButton();
  form->addRow(tr("Color"), _dcolor);

  // plot style
  Curve fac;
  fac.list(l, TRUE);

  _settings.getData(STOCHS::_SlowDPlot, d);

  _dplotStyle = new QComboBox;
  _dplotStyle->addItems(l);
  _dplotStyle->setCurrentIndex(_dplotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _dplotStyle);

  // label
  _settings.getData(STOCHS::_SlowDLabel, d);

  _dlabel = new QLineEdit(d);
  form->addRow(tr("Label"), _dlabel);

  _tabs->addTab(w, "%D");
}

void STOCHSDialog::createRefPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(STOCHS::_Ref1Color, d);
  QColor c(d);

  _refColor = new ColorButton(this, c);
  _refColor->setColorButton();
  form->addRow(tr("Color"), _refColor);

  // ref
  _ref = new QDoubleSpinBox;
  _ref->setRange(-100000, 100000);
  _ref->setValue(_settings.getDouble(STOCHS::_Ref1));
  form->addRow(tr("Value"), _ref);

  _tabs->addTab(w, tr("Ref 1"));
}

void STOCHSDialog::createRef2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(STOCHS::_Ref2Color, d);
  QColor c(d);

  _ref2Color = new ColorButton(this, c);
  _ref2Color->setColorButton();
  form->addRow(tr("Color"), _ref2Color);

  // ref
  _ref2 = new QDoubleSpinBox;
  _ref2->setRange(-100000, 100000);
  _ref2->setValue(_settings.getDouble(STOCHS::_Ref2));
  form->addRow(tr("Value"), _ref2);

  _tabs->addTab(w, tr("Ref 2"));
}

void STOCHSDialog::done ()
{
  _settings.setData(STOCHS::_SlowKColor, _kcolor->color().name());
  _settings.setData(STOCHS::_SlowDColor, _dcolor->color().name());
  _settings.setData(STOCHS::_SlowKPlot, _kplotStyle->currentText());
  _settings.setData(STOCHS::_SlowDPlot, _dplotStyle->currentText());
  _settings.setData(STOCHS::_SlowKLabel, _klabel->text());
  _settings.setData(STOCHS::_SlowDLabel, _dlabel->text());
  _settings.setData(STOCHS::_SlowDPeriod, _dperiod->value());
  _settings.setData(STOCHS::_SlowKPeriod, _kperiod->value());
  _settings.setData(STOCHS::_FastKPeriod, _fkperiod->value());
  _settings.setData(STOCHS::_SlowDMA, _dmaType->currentText());
  _settings.setData(STOCHS::_SlowKMA, _kmaType->currentText());
  _settings.setData(STOCHS::_Ref1, _ref->value());
  _settings.setData(STOCHS::_Ref2, _ref2->value());
  _settings.setData(STOCHS::_Ref1Color, _refColor->color().name());
  _settings.setData(STOCHS::_Ref2Color, _ref2Color->color().name());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

