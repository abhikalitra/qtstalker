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
#include <QLayout>
#include <QLabel>
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

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // candle color
  QLabel *label = new QLabel(tr("Candle Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(CANDLES::_Color, d);
  QColor c(d);

  _candleColor = new ColorButton(this, c);
  _candleColor->setColorButton();
  grid->addWidget(_candleColor, row++, col--);

  // method color
  label = new QLabel(tr("Method Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(CANDLES::_MethodColor, d);
  c.setNamedColor(d);

  _methodColor = new ColorButton(this, c);
  _methodColor->setColorButton();
  grid->addWidget(_methodColor, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CANDLES::_Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);

  // method
  label = new QLabel(tr("Method"));
  grid->addWidget(label, row, col++);

  CANDLES f;
  QStringList l = f.list();

  _settings.getData(CANDLES::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  grid->addWidget(_method, row++, col--);

  // penetration
  label = new QLabel(tr("Penetration"));
  grid->addWidget(label, row, col++);

  _pen = new QDoubleSpinBox;
  _pen->setRange(0, 100);
  _pen->setValue(_settings.getDouble(CANDLES::_Penetration));
  grid->addWidget(_pen, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("General"));
}

void CANDLESDialog::createMA1Page ()
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
  _settings.getData(CANDLES::_MAColor, d);
  QColor c(d);

  _ma1Color = new ColorButton(this, c);
  _ma1Color->setColorButton();
  grid->addWidget(_ma1Color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MAPlot, d);

  _ma1PlotStyle = new QComboBox;
  _ma1PlotStyle->addItems(l);
  _ma1PlotStyle->setCurrentIndex(_ma1PlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma1PlotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CANDLES::_MALabel, d);

  _ma1Label = new QLineEdit(d);
  grid->addWidget(_ma1Label, row++, col--);

  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _ma1Period = new QSpinBox;
  _ma1Period->setRange(1, 100000);
  _ma1Period->setValue(_settings.getInt(CANDLES::_MAPeriod));
  grid->addWidget(_ma1Period, row++, col--);

  // ma type
  label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MAType, d);

  _ma1Type = new QComboBox;
  _ma1Type->addItems(l);
  _ma1Type->setCurrentIndex(_ma1Type->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma1Type, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "MA 1");
}

void CANDLESDialog::createMA2Page ()
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
  _settings.getData(CANDLES::_MA2Color, d);
  QColor c(d);

  _ma2Color = new ColorButton(this, c);
  _ma2Color->setColorButton();
  grid->addWidget(_ma2Color, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MA2Plot, d);

  _ma2PlotStyle = new QComboBox;
  _ma2PlotStyle->addItems(l);
  _ma2PlotStyle->setCurrentIndex(_ma2PlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma2PlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CANDLES::_MA2Label, d);

  _ma2Label = new QLineEdit(d);
  grid->addWidget(_ma2Label, row++, col--);


  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _ma2Period = new QSpinBox;
  _ma2Period->setRange(1, 100000);
  _ma2Period->setValue(_settings.getInt(CANDLES::_MA2Period));
  grid->addWidget(_ma2Period, row++, col--);


  // ma type
  label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MA2Type, d);

  _ma2Type = new QComboBox;
  _ma2Type->addItems(l);
  _ma2Type->setCurrentIndex(_ma2Type->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma2Type, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "MA 2");
}

void CANDLESDialog::createMA3Page ()
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
  _settings.getData(CANDLES::_MA3Color, d);
  QColor c(d);

  _ma3Color = new ColorButton(this, c);
  _ma3Color->setColorButton();
  grid->addWidget(_ma3Color, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(CANDLES::_MA3Plot, d);

  _ma3PlotStyle = new QComboBox;
  _ma3PlotStyle->addItems(l);
  _ma3PlotStyle->setCurrentIndex(_ma3PlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma3PlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(CANDLES::_MA3Label, d);

  _ma3Label = new QLineEdit(d);
  grid->addWidget(_ma3Label, row++, col--);


  // period
  label = new QLabel(tr("Period"));
  grid->addWidget(label, row, col++);

  _ma3Period = new QSpinBox;
  _ma3Period->setRange(1, 100000);
  _ma3Period->setValue(_settings.getInt(CANDLES::_MA3Period));
  grid->addWidget(_ma3Period, row++, col--);


  // ma type
  label = new QLabel(tr("Type"));
  grid->addWidget(label, row, col++);

  FunctionMA mau;
  l = mau.list();

  _settings.getData(CANDLES::_MA3Type, d);

  _ma3Type = new QComboBox;
  _ma3Type->addItems(l);
  _ma3Type->setCurrentIndex(_ma3Type->findText(d, Qt::MatchExactly));
  grid->addWidget(_ma3Type, row++, col--);


  grid->setRowStretch(row, 1);

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

