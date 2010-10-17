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

#include "VOLDialog.h"
#include "VOL.h"
#include "FunctionMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

VOLDialog::VOLDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createMAPage();
}

void VOLDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // up color
  QString d;
  _settings.getData(VOL::UpColor, d);
  QColor c(d);

  _upColor = new ColorButton(this, c);
  _upColor->setColorButton();
  form->addRow(tr("Up Color"), _upColor);

  // down color
  _settings.getData(VOL::DownColor, d);
  c.setNamedColor(d);

  _downColor = new ColorButton(this, c);
  _downColor->setColorButton();
  form->addRow(tr("Down Color"), _downColor);

  // neutral color
  _settings.getData(VOL::NeutralColor, d);
  c.setNamedColor(d);

  _neutralColor = new ColorButton(this, c);
  _neutralColor->setColorButton();
  form->addRow(tr("Neutral Color"), _neutralColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(VOL::Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(VOL::Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);

  _tabs->addTab(w, tr("General"));
}

void VOLDialog::createMAPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(VOL::MAColor, d);
  QColor c(d);

  _maColor = new ColorButton(this, c);
  _maColor->setColorButton();
  form->addRow(tr("Color"), _maColor);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(VOL::MAPlot, d);

  _maPlotStyle = new QComboBox;
  _maPlotStyle->addItems(l);
  _maPlotStyle->setCurrentIndex(_maPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _maPlotStyle);

  // label
  _settings.getData(VOL::MALabel, d);

  _maLabel = new QLineEdit(d);
  form->addRow(tr("Label"), _maLabel);

  // period
  _maPeriod = new QSpinBox;
  _maPeriod->setRange(1, 100000);
  _maPeriod->setValue(_settings.getInt(VOL::MAPeriod));
  form->addRow(tr("Period"), _maPeriod);

  // ma type
  FunctionMA mau;
  l = mau.list();

  _settings.getData(VOL::MAType, d);

  _maType = new QComboBox;
  _maType->addItems(l);
  _maType->setCurrentIndex(_maType->findText(d, Qt::MatchExactly));
  form->addRow(tr("Type"), _maType);

  _tabs->addTab(w, tr("MA"));
}

void VOLDialog::done ()
{
  _settings.setData(VOL::UpColor, _upColor->color().name());
  _settings.setData(VOL::DownColor, _downColor->color().name());
  _settings.setData(VOL::NeutralColor, _neutralColor->color().name());
  _settings.setData(VOL::Label, _label->text());
  _settings.setData(VOL::Plot, _plotStyle->currentText());
  _settings.setData(VOL::MAColor, _maColor->color().name());
  _settings.setData(VOL::MAPlot, _maPlotStyle->currentText());
  _settings.setData(VOL::MALabel, _maLabel->text());
  _settings.setData(VOL::MAPeriod, _maPeriod->value());
  _settings.setData(VOL::MAType, _maType->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

