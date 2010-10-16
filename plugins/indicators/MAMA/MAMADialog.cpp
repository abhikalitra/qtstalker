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

#include "MAMADialog.h"
#include "MAMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

MAMADialog::MAMADialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMAMAPage();
  createFAMAPage();
  createOSCPage();
}

void MAMADialog::createGeneralPage ()
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
  _settings.getData(MAMA::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // fast limit
  _fast = new QDoubleSpinBox;
  _fast->setRange(0.01, 0.99);
  _fast->setValue(_settings.getDouble(MAMA::_FastLimit));
  form->addRow(tr("Fast Limit"), _fast);

  // slow limit
  _slow = new QDoubleSpinBox;
  _slow->setRange(0.01, 0.99);
  _slow->setValue(_settings.getDouble(MAMA::_SlowLimit));
  form->addRow(tr("Slow Limit"), _slow);

  // osc check
  _check = new QCheckBox;
  _check->setChecked(_settings.getInt(MAMA::_OSC));
  form->addRow(tr("Oscillator"), _check);
  
  _tabs->addTab(w, tr("General"));  
}

void MAMADialog::createMAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(MAMA::_MAMAColor, d);
  QColor c(d);

  _mamaColor = new ColorButton(this, c);
  _mamaColor->setColorButton();
  form->addRow(tr("Color"), _mamaColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::_MAMAPlot, d);

  _mamaPlotStyle = new QComboBox;
  _mamaPlotStyle->addItems(l);
  _mamaPlotStyle->setCurrentIndex(_mamaPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _mamaPlotStyle);
  
  // label
  _settings.getData(MAMA::_MAMALabel, d);

  _mamaLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _mamaLabel);

  _tabs->addTab(w, "MAMA");
}

void MAMADialog::createFAMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(MAMA::_FAMAColor, d);
  QColor c(d);

  _famaColor = new ColorButton(this, c);
  _famaColor->setColorButton();
  form->addRow(tr("Color"), _famaColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::_FAMAPlot, d);

  _famaPlotStyle = new QComboBox;
  _famaPlotStyle->addItems(l);
  _famaPlotStyle->setCurrentIndex(_famaPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _famaPlotStyle);

  // label
  _settings.getData(MAMA::_FAMALabel, d);

  _famaLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _famaLabel);

  _tabs->addTab(w, "FAMA");
}

void MAMADialog::createOSCPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(MAMA::_OSCColor, d);
  QColor c(d);

  _oscColor = new ColorButton(this, c);
  _oscColor->setColorButton();
  form->addRow(tr("Color"), _oscColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::_OSCPlot, d);

  _oscPlotStyle = new QComboBox;
  _oscPlotStyle->addItems(l);
  _oscPlotStyle->setCurrentIndex(_oscPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _oscPlotStyle);

  // up label
  _settings.getData(MAMA::_OSCLabel, d);

  _oscLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _oscLabel);

  _tabs->addTab(w, "OSC");
}

void MAMADialog::done ()
{
  _settings.setData(MAMA::_OSC, 1);
  _settings.setData(MAMA::_OSCColor, _oscColor->color().name());
  _settings.setData(MAMA::_MAMAColor, _mamaColor->color().name());
  _settings.setData(MAMA::_FAMAColor, _famaColor->color().name());
  _settings.setData(MAMA::_MAMAPlot, _mamaPlotStyle->currentText());
  _settings.setData(MAMA::_FAMAPlot, _famaPlotStyle->currentText());
  _settings.setData(MAMA::_OSCPlot, _oscPlotStyle->currentText());
  _settings.setData(MAMA::_MAMALabel, _mamaLabel->text());
  _settings.setData(MAMA::_FAMALabel, _famaLabel->text());
  _settings.setData(MAMA::_OSCLabel, _oscLabel->text());
  _settings.setData(MAMA::_FastLimit, _fast->value());
  _settings.setData(MAMA::_SlowLimit, _slow->value());
  _settings.setData(MAMA::_Input, _input->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

