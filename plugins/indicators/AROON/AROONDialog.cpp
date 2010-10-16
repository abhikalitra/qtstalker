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

#include "AROONDialog.h"
#include "AROON.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

AROONDialog::AROONDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createAROONPage();
  createOSCPage();
}

void AROONDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // method
  AROON f;
  QStringList l = f.list();

  QString d;
  _settings.getData(AROON::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);

  // period
  _period = new QSpinBox;
  _period->setRange(2, 100000);
  _period->setValue(_settings.getInt(AROON::_Period));
  form->addRow(tr("Period"), _period);
  
  _tabs->addTab(w, tr("General"));  
}

void AROONDialog::createAROONPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  QString d;
  _settings.getData(AROON::_UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _settings.getData(AROON::_DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // up plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AROON::_UpPlot, d);

  _upPlotStyle = new QComboBox;
  _upPlotStyle->addItems(l);
  _upPlotStyle->setCurrentIndex(_upPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Up Plot"), _upPlotStyle);
  
  // down plot style
  _settings.getData(AROON::_DownPlot, d);

  _downPlotStyle = new QComboBox;
  _downPlotStyle->addItems(l);
  _downPlotStyle->setCurrentIndex(_downPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Down Plot"), _downPlotStyle);

  // up label
  _settings.getData(AROON::_UpLabel, d);

  _upLabel = new QLineEdit(d);
  form->addRow(tr("Up Label"), _upLabel);

  // down label
  _settings.getData(AROON::_DownLabel, d);

  _downLabel = new QLineEdit(d);
  form->addRow(tr("Down Label"), _downLabel);

  _tabs->addTab(w, "AROON");
}

void AROONDialog::createOSCPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // osc color
  QString d;
  _settings.getData(AROON::_OSCColor, d);
  QColor c(d);

  _oscColor = new ColorButton(this, c);
  _oscColor->setColorButton();
  form->addRow(tr("Color"), _oscColor);

  // up plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(AROON::_OSCPlot, d);

  _oscPlotStyle = new QComboBox;
  _oscPlotStyle->addItems(l);
  _oscPlotStyle->setCurrentIndex(_oscPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _oscPlotStyle);

  // up label
  _settings.getData(AROON::_OSCLabel, d);

  _oscLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _oscLabel);

  _tabs->addTab(w, "OSC");
}

void AROONDialog::done ()
{
  _settings.setData(AROON::_Method, _method->currentText());
  _settings.setData(AROON::_DownColor, _downColor->color().name());
  _settings.setData(AROON::_UpColor, _upColor->color().name());
  _settings.setData(AROON::_DownPlot, _downPlotStyle->currentText());
  _settings.setData(AROON::_UpPlot, _upPlotStyle->currentText());
  _settings.setData(AROON::_DownLabel, _downLabel->text());
  _settings.setData(AROON::_UpLabel, _upLabel->text());
  _settings.setData(AROON::_OSCColor, _oscColor->color().name());
  _settings.setData(AROON::_OSCPlot, _oscPlotStyle->currentText());
  _settings.setData(AROON::_OSCLabel, _oscLabel->text());
  _settings.setData(AROON::_Period, _period->value());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

