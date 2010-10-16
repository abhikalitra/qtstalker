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

#include "BARSDialog.h"
#include "BARS.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

BARSDialog::BARSDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMA1Page();
  createMA2Page();
  createMA3Page();
}

void BARSDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  QString d;
  _settings.getData(BARS::_UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _settings.getData(BARS::_DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _settings.getData(BARS::_NeutralColor, d);
  c.setNamedColor(d);

  _neutralColor = new ColorButton(this, c);
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // label
  _settings.getData(BARS::_BarsLabel, d);

  _barsLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _barsLabel);

  _tabs->addTab(w, tr("BARS"));
}

void BARSDialog::createMA1Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BARS::_MAColor, d);
  QColor c(d);

  _ma1Color = new ColorButton(this, c);
  _ma1Color->setColorButton();
  form->addRow(tr("Color"), _ma1Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BARS::_MAPlot, d);

  _ma1PlotStyle = new QComboBox;
  _ma1PlotStyle->addItems(l);
  _ma1PlotStyle->setCurrentIndex(_ma1PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma1PlotStyle);

  // label
  _settings.getData(BARS::_MALabel, d);

  _ma1Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma1Label);

  // period
  _ma1Period = new QSpinBox;
  _ma1Period->setRange(1, 100000);
  _ma1Period->setValue(_settings.getInt(BARS::_MAPeriod));
  form->addRow(tr("Period"), _ma1Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(BARS::_MAType, d);

  _ma1Type = new QComboBox;
  _ma1Type->addItems(l);
  _ma1Type->setCurrentIndex(_ma1Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma1Type);

  _tabs->addTab(w, "MA 1");
}

void BARSDialog::createMA2Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BARS::_MA2Color, d);
  QColor c(d);

  _ma2Color = new ColorButton(this, c);
  _ma2Color->setColorButton();
  form->addRow(tr("Color"), _ma2Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BARS::_MA2Plot, d);

  _ma2PlotStyle = new QComboBox;
  _ma2PlotStyle->addItems(l);
  _ma2PlotStyle->setCurrentIndex(_ma2PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma2PlotStyle);

  // label
  _settings.getData(BARS::_MA2Label, d);

  _ma2Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma2Label);

  // period
  _ma2Period = new QSpinBox;
  _ma2Period->setRange(1, 100000);
  _ma2Period->setValue(_settings.getInt(BARS::_MA2Period));
  form->addRow(tr("Period"), _ma2Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(BARS::_MA2Type, d);

  _ma2Type = new QComboBox;
  _ma2Type->addItems(l);
  _ma2Type->setCurrentIndex(_ma2Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma2Type);

  _tabs->addTab(w, "MA 2");
}

void BARSDialog::createMA3Page ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(BARS::_MA3Color, d);
  QColor c(d);

  _ma3Color = new ColorButton(this, c);
  _ma3Color->setColorButton();
  form->addRow(tr("Color"), _ma3Color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(BARS::_MA3Plot, d);

  _ma3PlotStyle = new QComboBox;
  _ma3PlotStyle->addItems(l);
  _ma3PlotStyle->setCurrentIndex(_ma3PlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _ma3PlotStyle);

  // label
  _settings.getData(BARS::_MA3Label, d);

  _ma3Label = new QLineEdit(d);
  form->addRow(tr("Label"), _ma3Label);

  // period
  _ma3Period = new QSpinBox;
  _ma3Period->setRange(1, 100000);
  _ma3Period->setValue(_settings.getInt(BARS::_MA3Period));
  form->addRow(tr("Period"), _ma3Period);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(BARS::_MA3Type, d);

  _ma3Type = new QComboBox;
  _ma3Type->addItems(l);
  _ma3Type->setCurrentIndex(_ma3Type->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _ma3Type);

  _tabs->addTab(w, "MA 3");
}

void BARSDialog::done ()
{
  _settings.setData(BARS::_UpColor, _upColor->color().name());
  _settings.setData(BARS::_DownColor, _downColor->color().name());
  _settings.setData(BARS::_NeutralColor, _neutralColor->color().name());
  _settings.setData(BARS::_BarsLabel, _barsLabel->text());
  _settings.setData(BARS::_MAColor, _ma1Color->color().name());
  _settings.setData(BARS::_MA2Color, _ma2Color->color().name());
  _settings.setData(BARS::_MA3Color, _ma3Color->color().name());
  _settings.setData(BARS::_MAPlot, _ma1PlotStyle->currentText());
  _settings.setData(BARS::_MA2Plot, _ma2PlotStyle->currentText());
  _settings.setData(BARS::_MA3Plot, _ma3PlotStyle->currentText());
  _settings.setData(BARS::_MALabel, _ma1Label->text());
  _settings.setData(BARS::_MA2Label, _ma2Label->text());
  _settings.setData(BARS::_MA3Label, _ma3Label->text());
  _settings.setData(BARS::_MAPeriod, _ma1Period->value());
  _settings.setData(BARS::_MA2Period, _ma2Period->value());
  _settings.setData(BARS::_MA3Period, _ma3Period->value());
  _settings.setData(BARS::_MAType, _ma1Type->currentText());
  _settings.setData(BARS::_MA2Type, _ma2Type->currentText());
  _settings.setData(BARS::_MA3Type, _ma3Type->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}
