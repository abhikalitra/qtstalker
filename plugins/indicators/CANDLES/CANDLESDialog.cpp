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

#include "CANDLESDialog.h"
#include "CANDLES.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>


CANDLESDialog::CANDLESDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMA1Page();
  createMA2Page();
  createMA3Page();
}

void CANDLESDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // candle color
  QString d;
  _settings.getData(CANDLES::_Color, d);
  QColor c(d);

  _candleColor = new ColorButton(this, c);
  _candleColor->setColorButton();
  form->addRow(tr("Candle Color"), _candleColor);

  // method color
  _settings.getData(CANDLES::_MethodColor, d);
  c.setNamedColor(d);

  _methodColor = new ColorButton(this, c);
  _methodColor->setColorButton();
  form->addRow(tr("Method Color"), _methodColor);

  // label
  _settings.getData(CANDLES::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  // method
  CANDLES f;
  QStringList l = f.list();

  _settings.getData(CANDLES::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);

  // penetration
  _pen = new QDoubleSpinBox;
  _pen->setRange(0, 100);
  _pen->setValue(_settings.getDouble(CANDLES::_Penetration));
  form->addRow(tr("Penetration"), _pen);

  _tabs->addTab(w, tr("General"));
}

void CANDLESDialog::createMA1Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CANDLES::_MAColor, d);
  QColor c(d);

  _ma1Color = new ColorButton(this, c);
  _ma1Color->setColorButton();
  form->addRow(tr("Color"), _ma1Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MAPlot, d);

  _ma1PlotStyle = new QComboBox;
  _ma1PlotStyle->addItems(l);
  _ma1PlotStyle->setCurrentIndex(_ma1PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma1PlotStyle);

  // label
  _settings.getData(CANDLES::_MALabel, d);

  _ma1Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma1Label);

  // period
  _ma1Period = new QSpinBox;
  _ma1Period->setRange(1, 100000);
  _ma1Period->setValue(_settings.getInt(CANDLES::_MAPeriod));
  form->addRow(tr("Period"), _ma1Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MAType, d);

  _ma1Type = new QComboBox;
  _ma1Type->addItems(l);
  _ma1Type->setCurrentIndex(_ma1Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma1Type);

  _tabs->addTab(w, "MA 1");
}

void CANDLESDialog::createMA2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CANDLES::_MA2Color, d);
  QColor c(d);

  _ma2Color = new ColorButton(this, c);
  _ma2Color->setColorButton();
  form->addRow(tr("Color"), _ma2Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MA2Plot, d);

  _ma2PlotStyle = new QComboBox;
  _ma2PlotStyle->addItems(l);
  _ma2PlotStyle->setCurrentIndex(_ma2PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma2PlotStyle);

  // label
  _settings.getData(CANDLES::_MA2Label, d);

  _ma2Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma2Label);

  // period
  _ma2Period = new QSpinBox;
  _ma2Period->setRange(1, 100000);
  _ma2Period->setValue(_settings.getInt(CANDLES::_MA2Period));
  form->addRow(tr("Period"), _ma2Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MA2Type, d);

  _ma2Type = new QComboBox;
  _ma2Type->addItems(l);
  _ma2Type->setCurrentIndex(_ma2Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma2Type);

  _tabs->addTab(w, "MA 2");
}

void CANDLESDialog::createMA3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(CANDLES::_MA3Color, d);
  QColor c(d);

  _ma3Color = new ColorButton(this, c);
  _ma3Color->setColorButton();
  form->addRow(tr("Color"), _ma3Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MA3Plot, d);

  _ma3PlotStyle = new QComboBox;
  _ma3PlotStyle->addItems(l);
  _ma3PlotStyle->setCurrentIndex(_ma3PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma3PlotStyle);

  // label
  _settings.getData(CANDLES::_MA3Label, d);

  _ma3Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma3Label);

  // period
  _ma3Period = new QSpinBox;
  _ma3Period->setRange(1, 100000);
  _ma3Period->setValue(_settings.getInt(CANDLES::_MA3Period));
  form->addRow(tr("Period"), _ma3Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MA3Type, d);

  _ma3Type = new QComboBox;
  _ma3Type->addItems(l);
  _ma3Type->setCurrentIndex(_ma3Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma3Type);

  _tabs->addTab(w, "MA 3");
}

void CANDLESDialog::done ()
{
  _settings.setData(CANDLES::_Penetration, _pen->value());
  _settings.setData(CANDLES::_Method, _method->currentText());
  _settings.setData(CANDLES::_MethodColor, _methodColor->color().name());
  _settings.setData(CANDLES::_Color, _candleColor->color().name());
  _settings.setData(CANDLES::_Label, _label->text());
  _settings.setData(CANDLES::_MAColor, _ma1Color->color().name());
  _settings.setData(CANDLES::_MA2Color, _ma2Color->color().name());
  _settings.setData(CANDLES::_MA3Color, _ma3Color->color().name());
  _settings.setData(CANDLES::_MAPlot, _ma1PlotStyle->currentText());
  _settings.setData(CANDLES::_MA2Plot, _ma2PlotStyle->currentText());
  _settings.setData(CANDLES::_MA3Plot, _ma3PlotStyle->currentText());
  _settings.setData(CANDLES::_MALabel, _ma1Label->text());
  _settings.setData(CANDLES::_MA2Label, _ma2Label->text());
  _settings.setData(CANDLES::_MA3Label, _ma3Label->text());
  _settings.setData(CANDLES::_MAPeriod, _ma1Period->value());
  _settings.setData(CANDLES::_MA2Period, _ma2Period->value());
  _settings.setData(CANDLES::_MA3Period, _ma3Period->value());
  _settings.setData(CANDLES::_MAType, _ma1Type->currentText());
  _settings.setData(CANDLES::_MA2Type, _ma2Type->currentText());
  _settings.setData(CANDLES::_MA3Type, _ma3Type->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

