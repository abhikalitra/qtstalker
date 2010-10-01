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
#include <QLayout>
#include <QLabel>
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
  _settings.getData(HT::_Color, d);
  QColor c(d);

  _color = new ColorButton(this, c);
  _color->setColorButton();
  grid->addWidget(_color, row++, col--);

  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_Plot, d);

  _plotStyle = new QComboBox;
  _plotStyle->addItems(l);
  _plotStyle->setCurrentIndex(_plotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_plotStyle, row++, col--);

  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_Label, d);

  _label = new QLineEdit(d);
  grid->addWidget(_label, row++, col--);
  
  // input
  label = new QLabel(tr("Input"));
  grid->addWidget(label, row, col++);

  BarData bd;
  bd.getInputFields(l);

  _settings.getData(HT::_Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // method
  label = new QLabel(tr("Method"));
  grid->addWidget(label, row, col++);

  HT f;
  l = f.list();

  _settings.getData(HT::_Method, d);

  _method = new QComboBox;
  _method->addItems(l);
  _method->setCurrentIndex(_method->findText(d, Qt::MatchExactly));
  grid->addWidget(_method, row++, col--);

  grid->setRowStretch(row, 1);
  
  _tabs->addTab(w, tr("General"));  
}

void HTDialog::createPhasorPage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // phase color
  QLabel *label = new QLabel(tr("Phase Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(HT::_PhaseColor, d);
  QColor c(d);

  _phaseColor = new ColorButton(this, c);
  _phaseColor->setColorButton();
  grid->addWidget(_phaseColor, row++, col--);


  // down color
  label = new QLabel(tr("Quad Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_QuadColor, d);
  c.setNamedColor(d);

  _quadColor = new ColorButton(this, c);
  _quadColor->setColorButton();
  grid->addWidget(_quadColor, row++, col--);


  // phase plot style
  label = new QLabel(tr("Phase Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_PhasePlot, d);

  _phasePlotStyle = new QComboBox;
  _phasePlotStyle->addItems(l);
  _phasePlotStyle->setCurrentIndex(_phasePlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_phasePlotStyle, row++, col--);

  
  // quad plot style
  label = new QLabel(tr("Quad Plot"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_QuadPlot, d);

  _quadPlotStyle = new QComboBox;
  _quadPlotStyle->addItems(l);
  _quadPlotStyle->setCurrentIndex(_quadPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_quadPlotStyle, row++, col--);


  // phase label
  label = new QLabel(tr("Phase Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_PhaseLabel, d);

  _phaseLabel = new QLineEdit(d);
  grid->addWidget(_phaseLabel, row++, col--);


  // quad label
  label = new QLabel(tr("Quad Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_QuadLabel, d);

  _quadLabel = new QLineEdit(d);
  grid->addWidget(_quadLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, tr("Phasor"));
}

void HTDialog::createSinePage ()
{
  QWidget *w = new QWidget;

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // sine color
  QLabel *label = new QLabel(tr("Sine Color"));
  grid->addWidget(label, row, col++);

  QString d;
  _settings.getData(HT::_SineColor, d);
  QColor c(d);

  _sineColor = new ColorButton(this, c);
  _sineColor->setColorButton();
  grid->addWidget(_sineColor, row++, col--);


  // lead color
  label = new QLabel(tr("Lead Color"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_LeadColor, d);
  c.setNamedColor(d);

  _leadColor = new ColorButton(this, c);
  _leadColor->setColorButton();
  grid->addWidget(_leadColor, row++, col--);


  // sine plot style
  label = new QLabel(tr("Sine Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(HT::_SinePlot, d);

  _sinePlotStyle = new QComboBox;
  _sinePlotStyle->addItems(l);
  _sinePlotStyle->setCurrentIndex(_sinePlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_sinePlotStyle, row++, col--);


  // lead plot style
  label = new QLabel(tr("Lead Plot"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_LeadPlot, d);

  _leadPlotStyle = new QComboBox;
  _leadPlotStyle->addItems(l);
  _leadPlotStyle->setCurrentIndex(_leadPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_leadPlotStyle, row++, col--);


  // sine label
  label = new QLabel(tr("Sine Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_SineLabel, d);

  _sineLabel = new QLineEdit(d);
  grid->addWidget(_sineLabel, row++, col--);


  // lead label
  label = new QLabel(tr("Lead Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(HT::_LeadLabel, d);

  _leadLabel = new QLineEdit(d);
  grid->addWidget(_leadLabel, row++, col--);


  grid->setRowStretch(row, 1);

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

