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

#include "HTDialog.h"
#include "HT.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QFormLayout>
#include <QStringList>

HTDialog::HTDialog (Indicator &i) : IndicatorPluginDialog (i)
{
  createGeneralPage();
  createPhasorPage();
  createSinePage();
}

void HTDialog::createGeneralPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // color
  QString d;
  _settings.getData(HT::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  form->addRow(tr("Color"), _color);

  // plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Plot"), _plotStyle);

  // label
  _settings.getData(HT::_Label, d);

  _label = new QLineEdit(d);
  form->addRow(tr("Label"), _label);
  
  // input
  BarData bd;
  bd.getInputFields(l);

  _settings.getData(HT::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  form->addRow(tr("Input"), _input);

  // method
  HT f;
  l = f.list();

  _settings.getData(HT::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  form->addRow(tr("Method"), _method);
  
  _tabs->addTab(w, tr("General"));  
}

void HTDialog::createPhasorPage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // phase color
  QString d;
  _settings.getData(HT::_PhaseColor, d);
  QColor c(d);

  _phaseColor = new ColorButton(this, c);
  _phaseColor->setColorButton();
  form->addRow(tr("Phase Color"), _phaseColor);

  // down color
  _settings.getData(HT::_QuadColor, d);
  c.setNamedColor(d);

  _quadColor = new ColorButton(this, c);
  _quadColor->setColorButton();
  form->addRow(tr("Quad Color"), _quadColor);

  // phase plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_PhasePlot, d);

  _phasePlotStyle = new QComboBox;
  _phasePlotStyle->addItems(l);
  _phasePlotStyle->setCurrentIndex(_phasePlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Phase Plot"), _phasePlotStyle);
  
  // quad plot style
  _settings.getData(HT::_QuadPlot, d);

  _quadPlotStyle = new QComboBox;
  _quadPlotStyle->addItems(l);
  _quadPlotStyle->setCurrentIndex(_quadPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Quad Plot"), _quadPlotStyle);

  // phase label
  _settings.getData(HT::_PhaseLabel, d);

  _phaseLabel = new QLineEdit(d);
  form->addRow(tr("Phase Label"), _phaseLabel);

  // quad label
  _settings.getData(HT::_QuadLabel, d);

  _quadLabel = new QLineEdit(d);
  form->addRow(tr("Quad Label"), _quadLabel);

  _tabs->addTab(w, tr("Phasor"));
}

void HTDialog::createSinePage ()
{
  QWidget *w = new QWidget;

  QFormLayout *form = new QFormLayout;
  form->setSpacing(2);
  form->setMargin(5);
  w->setLayout(form);

  // sine color
  QString d;
  _settings.getData(HT::_SineColor, d);
  QColor c(d);

  _sineColor = new ColorButton(this, c);
  _sineColor->setColorButton();
  form->addRow(tr("Sine Color"), _sineColor);

  // lead color
  _settings.getData(HT::_LeadColor, d);
  c.setNamedColor(d);

  _leadColor = new ColorButton(this, c);
  _leadColor->setColorButton();
  form->addRow(tr("Lead Color"), _leadColor);

  // sine plot style
  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_SinePlot, d);

  _sinePlotStyle = new QComboBox;
  _sinePlotStyle->addItems(l);
  _sinePlotStyle->setCurrentIndex(_sinePlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Sine Plot"), _sinePlotStyle);

  // lead plot style
  _settings.getData(HT::_LeadPlot, d);

  _leadPlotStyle = new QComboBox;
  _leadPlotStyle->addItems(l);
  _leadPlotStyle->setCurrentIndex(_leadPlotStyle->findText(d, Qt::MatchExactly));
  form->addRow(tr("Lead Plot"), _leadPlotStyle);

  // sine label
  _settings.getData(HT::_SineLabel, d);

  _sineLabel = new QLineEdit(d);
  form->addRow(tr("Sine Label"), _sineLabel);

  // lead label
  _settings.getData(HT::_LeadLabel, d);

  _leadLabel = new QLineEdit(d);
  form->addRow(tr("Lead Label"), _leadLabel);

  _tabs->addTab(w, tr("Sine"));
}

void HTDialog::done ()
{
  _settings.setData(HT::_Method, _method->currentText());
  _settings.setData(HT::_Color, _color->color().name());
  _settings.setData(HT::_PhaseColor, _phaseColor->color().name());
  _settings.setData(HT::_QuadColor, _quadColor->color().name());
  _settings.setData(HT::_SineColor, _sineColor->color().name());
  _settings.setData(HT::_LeadColor, _leadColor->color().name());
  _settings.setData(HT::_Plot, _plotStyle->currentText());
  _settings.setData(HT::_PhasePlot, _phasePlotStyle->currentText());
  _settings.setData(HT::_QuadPlot, _quadPlotStyle->currentText());
  _settings.setData(HT::_SinePlot, _sinePlotStyle->currentText());
  _settings.setData(HT::_LeadPlot, _leadPlotStyle->currentText());
  _settings.setData(HT::_Label, _label->text());
  _settings.setData(HT::_PhaseLabel, _phaseLabel->text());
  _settings.setData(HT::_QuadLabel, _quadLabel->text());
  _settings.setData(HT::_SineLabel, _sineLabel->text());
  _settings.setData(HT::_LeadLabel, _leadLabel->text());
  _settings.setData(HT::_Input, _input->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

