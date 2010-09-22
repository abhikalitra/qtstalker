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
#include "FunctionMAMA.h"
#include "MAMA.h"
#include "Curve.h"
#include "IndicatorDataBase.h"
#include "BarData.h"

#include <QtDebug>
#include <QLayout>
#include <QLabel>
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

  QGridLayout *grid = new QGridLayout;
  grid->setSpacing(2);
  grid->setColumnStretch(1, 1);
  w->setLayout(grid);

  int row = 0;
  int col = 0;

  // input
  QLabel *label = new QLabel(tr("Input"));
  grid->addWidget(label, row, col++);

  BarData bd;
  QStringList l;
  bd.getInputFields(l);

  QString d;
  _settings.getData(MAMA::Input, d);

  _input = new QComboBox;
  _input->addItems(l);
  _input->setCurrentIndex(_input->findText(d, Qt::MatchExactly));
  grid->addWidget(_input, row++, col--);

  // fast limit
  label = new QLabel(tr("Fast Limit"));
  grid->addWidget(label, row, col++);

  _fast = new QDoubleSpinBox;
  _fast->setRange(0.01, 0.99);
  _fast->setValue(_settings.getDouble(MAMA::FastLimit));
  grid->addWidget(_fast, row++, col--);

  // slow limit
  label = new QLabel(tr("Slow Limit"));
  grid->addWidget(label, row, col++);

  _slow = new QDoubleSpinBox;
  _slow->setRange(0.01, 0.99);
  _slow->setValue(_settings.getDouble(MAMA::SlowLimit));
  grid->addWidget(_slow, row++, col--);

  // osc check
  _check = new QCheckBox(tr("Oscillator"));
  _check->setChecked(_settings.getInt(MAMA::OSC));
  grid->addWidget(_check, row++, col);
  

  grid->setRowStretch(row, 1);
  
  _tabs->addTab(w, tr("General"));  
}

void MAMADialog::createMAMAPage ()
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
  _settings.getData(MAMA::MAMAColor, d);
  QColor c(d);

  _mamaColor = new ColorButton(this, c);
  _mamaColor->setColorButton();
  grid->addWidget(_mamaColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::MAMAPlot, d);

  _mamaPlotStyle = new QComboBox;
  _mamaPlotStyle->addItems(l);
  _mamaPlotStyle->setCurrentIndex(_mamaPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_mamaPlotStyle, row++, col--);

  
  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAMA::MAMALabel, d);

  _mamaLabel = new QLineEdit(d);
  grid->addWidget(_mamaLabel, row++, col--);


  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "MAMA");
}

void MAMADialog::createFAMAPage ()
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
  _settings.getData(MAMA::FAMAColor, d);
  QColor c(d);

  _famaColor = new ColorButton(this, c);
  _famaColor->setColorButton();
  grid->addWidget(_famaColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::FAMAPlot, d);

  _famaPlotStyle = new QComboBox;
  _famaPlotStyle->addItems(l);
  _famaPlotStyle->setCurrentIndex(_famaPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_famaPlotStyle, row++, col--);


  // label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAMA::FAMALabel, d);

  _famaLabel = new QLineEdit(d);
  grid->addWidget(_famaLabel, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "FAMA");
}

void MAMADialog::createOSCPage ()
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
  _settings.getData(MAMA::OSCColor, d);
  QColor c(d);

  _oscColor = new ColorButton(this, c);
  _oscColor->setColorButton();
  grid->addWidget(_oscColor, row++, col--);


  // plot style
  label = new QLabel(tr("Plot"));
  grid->addWidget(label, row, col++);

  Curve fac;
  QStringList l;
  fac.list(l, TRUE);

  _settings.getData(MAMA::OSCPlot, d);

  _oscPlotStyle = new QComboBox;
  _oscPlotStyle->addItems(l);
  _oscPlotStyle->setCurrentIndex(_oscPlotStyle->findText(d, Qt::MatchExactly));
  grid->addWidget(_oscPlotStyle, row++, col--);


  // up label
  label = new QLabel(tr("Label"));
  grid->addWidget(label, row, col++);

  _settings.getData(MAMA::OSCLabel, d);

  _oscLabel = new QLineEdit(d);
  grid->addWidget(_oscLabel, row++, col--);

  grid->setRowStretch(row, 1);

  _tabs->addTab(w, "OSC");
}

void MAMADialog::done ()
{
  _settings.setData(MAMA::OSC, 1);
  _settings.setData(MAMA::OSCColor, _oscColor->color().name());
  _settings.setData(MAMA::MAMAColor, _mamaColor->color().name());
  _settings.setData(MAMA::FAMAColor, _famaColor->color().name());
  _settings.setData(MAMA::MAMAPlot, _mamaPlotStyle->currentText());
  _settings.setData(MAMA::FAMAPlot, _famaPlotStyle->currentText());
  _settings.setData(MAMA::OSCPlot, _oscPlotStyle->currentText());
  _settings.setData(MAMA::MAMALabel, _mamaLabel->text());
  _settings.setData(MAMA::FAMALabel, _famaLabel->text());
  _settings.setData(MAMA::OSCLabel, _oscLabel->text());
  _settings.setData(MAMA::FastLimit, _fast->value());
  _settings.setData(MAMA::SlowLimit, _slow->value());
  _settings.setData(MAMA::Input, _input->currentText());

  _indicator.setSettings(_settings);

  IndicatorDataBase db;
  db.transaction();
  db.setIndicator(_indicator);
  db.commit();

  emit signalDone(_indicator);

  accept();
}

